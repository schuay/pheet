/*
 * BasicLinkedListStrategyTaskStorage.h
 *
 *  Created on: 06.04.2012
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef BASICLINKEDLISTSTRATEGYTASKSTORAGE_H_
#define BASICLINKEDLISTSTRATEGYTASKSTORAGE_H_

#include "BasicLinkedListStrategyTaskStoragePerformanceCounters.h"
#include "BasicLinkedListStrategyTaskStorageDataBlock.h"
#include "BasicLinkedListStrategyTaskStorageView.h"
#include "BasicLinkedListStrategyTaskStorageStream.h"
#include "../../StrategyHeap/Basic/BasicStrategyHeap.h"
//#include "../../StrategyHeap/Volatile2/VolatileStrategyHeap2.h"
#include "../../../misc/type_traits.h"
#include <pheet/memory/ItemReuse/ItemReuseMemoryManager.h>
#include <pheet/ds/PriorityQueue/FibonacciSame/FibonacciSameHeap.h>

#include <vector>
#include <queue>
#include <iostream>

namespace pheet {

template <class Pheet, typename TT, class TaskStorage, class Stream, class StealerRef>
struct BasicLinkedListStrategyTaskStorageItem {
	typedef TT Item;

	typename Pheet::Scheduler::BaseStrategy* strategy;
	TT item;
	size_t taken;
	void (Stream::*stealer_push)(StealerRef& stealer);
	void (Stream::*task_storage_push)(TaskStorage& task_storage, typename Pheet::Scheduler::BaseStrategy* strategy, TT const& stream_ref);
};

template <class Pheet, typename DataBlock>
struct BasicLinkedListStrategyTaskStorageLocalReference {
	DataBlock* block;
	size_t index;
};

template <class Pheet, class TaskStorage>
class BasicLinkedListStrategyTaskStorageStrategyRetriever {
public:
	typedef BasicLinkedListStrategyTaskStorageStrategyRetriever<Pheet, TaskStorage> Self;
	typedef typename TaskStorage::LocalRef Item;

	BasicLinkedListStrategyTaskStorageStrategyRetriever(TaskStorage* task_storage)
	:task_storage(task_storage) {}

	BasicLinkedListStrategyTaskStorageStrategyRetriever(BasicLinkedListStrategyTaskStorageStrategyRetriever const& other)
	:task_storage(other.task_storage) {}

	BasicLinkedListStrategyTaskStorageStrategyRetriever(BasicLinkedListStrategyTaskStorageStrategyRetriever&& other)
	:task_storage(other.task_storage) {}

	typename Pheet::Scheduler::BaseStrategy* operator()(Item const& item) {
		return (item.block->get_data(item.index).strategy);
	}

	inline bool is_active(Item& item) {
		return (item.block->get_data(item.index).taken & 1) == 0;
	}

	inline void mark_removed(Item& item) {
		item.block->mark_removed(item.index, task_storage->get_current_view());
	}

private:
	TaskStorage* task_storage;
};

template <class View>
struct BasicLinkedListStrategyTaskStorageViewReuseCheck {
	bool operator()(View& v) {
		return v.try_reuse();
	}
};

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
class BasicLinkedListStrategyTaskStorageImpl {
public:
	typedef BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize> Self;

	typedef TT T;
	typedef StealerT<Pheet, Self> Stealer;
	typedef typename Stealer::StealerRef StealerRef;
	typedef BasicLinkedListStrategyTaskStorageStream<Pheet, Self, StealerRef> Stream;
	typedef BasicLinkedListStrategyTaskStorageItem<Pheet, T, Self, Stream, StealerRef> Item;
	typedef BasicLinkedListStrategyTaskStorageView<Pheet, Item, BlockSize> View;
	typedef BasicLinkedListStrategyTaskStorageDataBlock<Pheet, Item, View, BlockSize> DataBlock;

	typedef BasicLinkedListStrategyTaskStorageLocalReference<Pheet, DataBlock> LocalRef;
	typedef BasicLinkedListStrategyTaskStorageStrategyRetriever<Pheet, Self> StrategyRetriever;

	typedef StrategyHeapT<Pheet, LocalRef, StrategyRetriever> StrategyHeap;
	typedef BasicLinkedListStrategyTaskStoragePerformanceCounters<Pheet, typename StrategyHeap::PerformanceCounters>
		PerformanceCounters;

	typedef ItemReuseMemoryManager<Pheet, View, BasicLinkedListStrategyTaskStorageViewReuseCheck<View> > ViewMemoryManager;

	template <template <class, typename, class> class NewStrategyHeap>
	using WithStrategyHeap = BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, NewStrategyHeap, BlockSize>;

	template <class NewPheet, typename NewTT>
	using BT = BasicLinkedListStrategyTaskStorageImpl<NewPheet, NewTT, StealerT, StrategyHeapT, BlockSize>;

	BasicLinkedListStrategyTaskStorageImpl();
	BasicLinkedListStrategyTaskStorageImpl(PerformanceCounters& pc);
	~BasicLinkedListStrategyTaskStorageImpl();

	template <class Strategy>
	void push(Strategy&& s, T item);
	T pop();
	void make_empty();
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

	inline View* get_current_view() { return current_view; }
	View* acquire_current_view() {
		View* ret;
		do {
			ret = current_view;
		} while(!ret->try_register());
		return ret;
	}

	static void print_name() {
		std::cout << "BasicLinkedListStrategyTaskStorage";
	}
private:

	void check_view();

	ViewMemoryManager views;
	View* current_view;

	DataBlock* back;
	size_t back_index;

	PerformanceCounters pc;
	StrategyHeap heap;
	std::vector<DataBlock*> data_block_reuse;
};

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize>::BasicLinkedListStrategyTaskStorageImpl()
:heap(StrategyRetriever(this), pc.strategy_heap_performance_counters){
	current_view = &(views.acquire_item());
	current_view->init_first(new DataBlock(0, nullptr), &data_block_reuse);
	back = current_view->get_front();
}

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize>::BasicLinkedListStrategyTaskStorageImpl(PerformanceCounters& pc)
:pc(pc), heap(StrategyRetriever(this), this->pc.strategy_heap_performance_counters){
	current_view = &(views.acquire_item());
	current_view->init_first(new DataBlock(0, nullptr), &data_block_reuse);
	back = current_view->get_front();
}

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize>::~BasicLinkedListStrategyTaskStorageImpl() {
	pheet_assert(heap.is_empty());

//	pheet_assert(current_view->get_front() == back);
//	pheet_assert(back->is_first());

	// Back may have up to log n predecessors
	back->delete_predecessors();
	delete back;

	for(auto i = data_block_reuse.begin(); i != data_block_reuse.end(); ++i) {
		delete *i;
	}

	// Views are automatically deleted by the memory manager

/*	DataBlock* tmp = front;

	while (tmp != back) {
		DataBlock* next = tmp->get_next();
		delete tmp;
		tmp = next;
	}
	delete tmp;*/
}

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
template <class Strategy>
void BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize>::push(Strategy&& s, T item) {
	Item it;
	it.strategy = new Strategy(std::move(s));
	it.item = item;
	it.taken = back->get_taken_offset();
	it.stealer_push = &Stream::template stealer_push_ref<Strategy>;
	it.task_storage_push = &Stream::template task_storage_push<Strategy>;

	LocalRef r;
	r.block = back;
	r.index = back->push(std::move(it), &data_block_reuse);
	heap.template push<Strategy>(std::move(r));

	if(back->get_next() != nullptr) {
		back = back->get_next();
	}
}

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
TT BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize>::pop() {
	while(heap.size() > 0) {
		LocalRef r = heap.pop();

		T ret;
		if(r.block->local_take(r.index, ret, current_view)) {
			check_view();
			return ret;
		}
	}
	check_view();
	return nullable_traits<T>::null_value;
}

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize>::make_empty() {
	while(!heap.is_empty()) {
		LocalRef r = heap.pop();

		pheet_assert(r.block->is_taken(r.index));
		r.block->mark_removed(r.index, current_view);
	}
}

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
TT& BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize>::peek() {
	auto r = heap.peek();
	return r.block->peek(r.index);
}

template <class Pheet, typename TT, template <class, class> class StealerT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageImpl<Pheet, TT, StealerT, StrategyHeapT, BlockSize>::check_view() {
	if(current_view->needs_cleanup()) {
//		check_blocked_freed_views();
		View* next_view = &(views.acquire_item());

		// Has a fence inside
		next_view->reset(current_view);

		current_view = next_view;
	}
}

template <class Pheet, typename T, template <class, class> class StealerT>
using BasicLinkedListStrategyTaskStorage = BasicLinkedListStrategyTaskStorageImpl<Pheet, T, StealerT, BasicStrategyHeap, 256>;

}

#endif /* BASICLINKEDLISTSTRATEGYTASKSTORAGE_H_ */
