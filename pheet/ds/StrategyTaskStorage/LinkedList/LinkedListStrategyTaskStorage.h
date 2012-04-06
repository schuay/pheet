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
#include <queue>

namespace pheet {

template <class Pheet, typename TT>
struct LinkedListStrategyTaskStorageItem {
	typename Pheet::Scheduler::BaseStrategy* strategy;
	TT item;
};

template <class Pheet, class Item>
struct LinkedListStrategyTaskStorageStrategyRetriever {
	typename Pheet::Scheduler::BaseStrategy& operator()(Item& item) {
		return *(item.strategy);
	}
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
	~LinkedListStrategyTaskStorage();

	template <class Strategy>
	void push(Strategy&& s, T item);
	T pop();
	T& peek();

	inline size_t size() {
		return get_length();
	}
	size_t get_length();
	inline bool empty() {
		return is_empty();
	}
	bool is_empty();
	inline bool is_full() {
		return false;
	}

	static void print_name();
private:
	void clean_block(DataBlock* block);

	// Entry point for stealers - acquire view and on success access front
	View* current_view;

//	DataBlock* front;
	DataBlock* back;

	StrategyHeap heap;

	// Linear dependencies between views - view may only be reused after predecessor is reused
	std::queue<View*> view_reuse;
};

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::LinkedListStrategyTaskStorage() {
	current_view = new View();
	front = new DataBlock(0, current_view, nullptr);
	back = front;
}

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::~LinkedListStrategyTaskStorage() {
	for(auto i = view_reuse.begin(); i != view_reuse.end(); ++i) {
		delete *i;
	}

	DataBlock* tmp = current_view->front;

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
	heap.push<Strategy>(it);
	++sz;
}

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
void LinkedListStrategyTaskStorage<Pheet, TT, StrategyHeapT, BlockSize>::clean_block(DataBlock* block) {
	// There should be no problems with id wraparound, except that on two blocks with identical ids
	// the predecessor won't be removed until the successor has been removed.
	// This means O(log(n)) is not satisfied in this (rare) case

	while(block->next != nullptr // Only free blocks if they have a successor
			&& block->freed == BlockSize // Check if block is not used any more
			&& (block->prev == nullptr // Either free block if it is the front block
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
