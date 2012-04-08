/*
 * LinkedListStrategyTaskStorage.h
 *
 *  Created on: 06.04.2012
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef LINKEDLISTSTRATEGYTASKSTORAGE_H_
#define LINKEDLISTSTRATEGYTASKSTORAGE_H_

#include "LinkedListStrategyTaskStoragePerformanceCounters.h"
#include "../../../misc/type_traits.h"
#include <queue>

namespace pheet {

template <class Pheet, typename TT>
struct LinkedListStrategyTaskStorageItem {
	typename Pheet::Scheduler::BaseStrategy* strategy;
	TT item;
	int taken;
};

template <class Pheet, typename DataBlock>
struct LinkedListStrategyTaskStorageLocalReference {
	DataBlock* block;
	size_t index;
};

template <class Pheet, class TaskStorage>
class LinkedListStrategyTaskStorageStrategyRetriever {
public:
	typedef typename TaskStorage::LocalReference Item;

	LinkedListStrategyTaskStorageStrategyRetriever(TaskStorage* task_storage)
	:task_storage(task_storage) {}


	typename Pheet::Scheduler::BaseStrategy& operator()(Item& item) {
		return *(item.block.data[item.index].strategy);
	}

	inline bool is_active(Item& item) {
		return item.block.data[item.index].taken != 0;
	}

	inline void mark_removed(Item& item) {
		pheet_assert(item.block->active > 0);
		--(item.block->active);
		if(item.block->active == 0) {
			task_storage->clean_block(item.block);
		}
	}

private:
	TaskStorage* task_storage
};

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
class LinkedListStrategyTaskStorage {
public:
	typedef TT T;
	typedef LinkedListStrategyTaskStorageItem<Pheet, T> Item;
	typedef LinkedListStrategyTaskStorageStrategyRetriever<Pheet, Item> StrategyRetriever;
	typedef StrategyHeapT<Pheet, Item, StrategyRetriever> StrategyHeap;
	typedef LinkedListStrategyTaskStoragePerformanceCounters<Pheet, typename StrategyHeap::PerformanceCounters>
		PerformanceCounters;

	LinkedListStrategyTaskStorage();
	LinkedListStrategyTaskStorage(PerformanceCounters& pc);
	~LinkedListStrategyTaskStorage();

	template <class Strategy>
	void push(Strategy&& s, T item);
	T pop();
	T& peek();

	inline size_t size() {
		return get_length();
	}
	size_t get_length() { return heap.size(); }
	inline bool empty() {
		return is_empty();
	}
	bool is_empty() { return heap.is_empty(); }
	inline bool is_full() {
		return false;
	}

	static void print_name();

	void clean_block(DataBlock* block);
private:

	View* current_view;

	DataBlock* front;
	DataBlock* back;
	size_t back_index;

	PerformanceCounters pc;
	StrategyHeap heap;

	std::vector<View*> view_reuse;
	std::vector<View*> blocked_freed_views;
};

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::LinkedListStrategyTaskStorage()
:heap(StrategyRetriever(this), pc.strategy_heap_performance_counters){
	current_view = new View();
	front = new DataBlock(0, current_view, nullptr);
	back = front;
}

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::LinkedListStrategyTaskStorage(PerformanceCounters& pc)
:pc(pc), heap(StrategyRetriever(this), this->pc.strategy_heap_performance_counters){
	current_view = new View();
	front = new DataBlock(0, current_view, nullptr);
	back = front;
}

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::~LinkedListStrategyTaskStorage() {
	for(auto i = view_reuse.begin(); i != view_reuse.end(); ++i) {
		delete *i;
	}

	DataBlock* tmp = front;

	while (tmp != back) {
		DataBlock* next = tmp->next;
		delete tmp;
		tmp = next;
	}
	delete tmp;

	delete current_view;
}

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
template <class Strategy>
void LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::push(Strategy&& s, T item) {
	Item it;
	it.strategy = new Strategy(std::move(s));
	it.item = item;
	it.taken = 0;

	back->data[back->filled] = it;

	LocalRef r;
	r.block = back;
	r.index = back->filled;
	heap.push<Strategy>(std::move(r));

	if(back->filled == BlockSize - 1) {
		DataBlock* tmp = new DataBlock(back->id + 1, current_view, back);
		MEMORY_FENCE();
		back->next = tmp;
		back = tmp;
	}
	else {
		MEMORY_FENCE();
	}
	++(back->filled);
}

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
T LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::pop() {
	while(heap.size() > 0) {
		LocalRef r = heap.pop();

		pheet_assert(r.block->active > 0);
		--(r.block->active);

		if(r.block->data[r.index].taken == 0) {
			if(INT_CAS(&(r.block->data[r.index].taken), 0, 1)) {
				T ret = r.block->data[r.index].item;
				if(r.block->active == 0) {
					clean_block(r.block);
				}
				return ret;
			}
		}
		if(r.block->active == 0) {
			clean_block(r.block);
		}
	}
	return nullable_traits<T>::null_value;
}

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
T& LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::peek() {
	auto r = heap.peek();
	return r.block->data[r.index].item;
}

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
void LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::clean_block(DataBlock* block) {
	// There should be no problems with id wraparound, except that block with id 0 ignores rules
	// This means O(log(n)) is not satisfied in for every predecessor of block 0
	// This leads to a complexity of O((n/(size_t_max + 1))*log(size_t_max)) for n > size_t max which isn't really bad
	// and rare in any case (it's linear to the number of wraparounds and not logarithmic)

	while(block->next != nullptr // Only free blocks if they have a successor
			&& block->freed == BlockSize // Check if block is not used any more
			&& (block->id // Either free block if has id 0 (special case)
					// Or make sure the next block is a higher power of two (to guarantee O(log(n)) access times
					// for other threads missing elements (don't want them to have O(n) for elements they never need)
					|| (block->next->id & block->id != block->next->id))) {
		block->next->prev = block->prev;
		current_view->empty_blocks.push(block);
		if(block->prev == nullptr) {
			current_view->front = block->next;
			break;
		}
		block->prev->next = block->next;
		block = block->prev;
	}
}

}

#endif /* LINKEDLISTSTRATEGYTASKSTORAGE_H_ */
