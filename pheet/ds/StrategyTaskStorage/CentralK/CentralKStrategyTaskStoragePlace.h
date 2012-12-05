/*
 * CentralKStrategyTaskStoragePlace.h
 *
 *  Created on: 24.10.2012
 *      Author: mwimmer
 */

#ifndef CENTRALKSTRATEGYTASKSTORAGEPLACE_H_
#define CENTRALKSTRATEGYTASKSTORAGEPLACE_H_

#include "CentralKStrategyTaskStorageDataBlock.h"
#include "CentralKStrategyTaskStorageItem.h"

#include <pheet/memory/ItemReuse/ItemReuseMemoryManager.h>

namespace pheet {

template <class Pheet, class Item>
struct CentralKStrategyTaskStorageItemReference {
	Item* item;
	size_t position;
	// If strategy equals item->strategy => locally created
	typename Pheet::Scheduler::BaseStrategy* strategy;
};

template <class Pheet, class Ref>
class CentralKStrategyTaskStorageStrategyRetriever {
public:
	typedef BasicLinkedListStrategyTaskStorageStrategyRetriever<Pheet, Ref> Self;

	CentralKStrategyTaskStorageStrategyRetriever() {}

	typename Pheet::Scheduler::BaseStrategy* operator()(Ref const& ref) {
		return ref.strategy;
	}

	inline bool is_active(Ref const& ref) {
		return ref.item.position == ref.position;
	}

	inline void mark_removed(Item& item) {
	//	item.block->mark_removed(item.index, task_storage->get_current_view());
	}

private:
};

template <class Pheet, class TaskStorage, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize, size_t Tests, bool LocalKPrio>
class CentralKStrategyTaskStoragePlace {
public:
	typedef CentralKStrategyTaskStoragePlace<Pheet, TaskStorage, TT, StrategyHeapT, BlockSize, Tests> Self;

	typedef CentralKStrategyTaskStorageDataBlock<Pheet, Self, TT, BlockSize, Tests> DataBlock;

	typedef TT T;
	typedef CentralKStrategyTaskStorageItem<Pheet, Self, TT> Item;
	typedef CentralKStrategyTaskStorageItemReference<Pheet, Item> Ref;
	typedef CentralKStrategyTaskStorageStrategyRetriever<Pheet, Ref> StrategyRetriever;

	typedef StrategyHeapT<Pheet, Ref, StrategyRetriever> StrategyHeap;

	typedef ItemReuseMemoryManager<Pheet, Item, CentralKStrategyTaskStorageItemReuseCheck<Item> > ItemMemoryManager;
	typedef ItemReuseMemoryManager<Pheet, DataBlock, CentralKStrategyTaskStorageDataBlockReuseCheck<DataBlock> > DataBlockMemoryManager;

	CentralKStrategyTaskStoragePlace(TaskStorage* task_storage)
	:task_storage(task_storage), head(0) {
		DataBlock* tmp = task_storage->start_block;
		if(tmp == nullptr) {
			// This assumes the first place is initialized before all others, otherwise synchronization would be needed!
			tmp = &(data_blocks.acquire_item());
			tmp->init_first(task_storage->get_num_places());
			task_storage->start_block = tmp;
		}
		tail_block = tmp;
		head_block = tmp;
	}

	~CentralKStrategyTaskStoragePlace() {
		while(!heap.empty()) {
			Ref r = heap.pop();
			// All items should have been processed
			pheet_assert(r.position != r.item->position);
			delete r.item->strategy;
			r.item->strategy = nullptr;
		}
	}

	template <class Strategy>
	void push(Strategy&& s, T data) {
		Item& it = items.acquire_item();
		pheet_assert(it.strategy == nullptr);
		it.strategy = new Strategy(s);
		it.data = data;
		it.item_push = &Self::template item_push<Strategy>;

		while(!tail_block->put(&(task_storage->head), &(task_storage->tail), &it)) {
			if(tail_block->get_next() == nullptr) {
				DataBlock& next_block = data_blocks.acquire_item();
				tail_block->add_block(&next_block, task_storage->get_num_places());
			}
			pheet_assert(tail_block->get_next() != nullptr);
			tail_block = tail_block->get_next();
		}

		Ref r;
		r.item = it;
		r.position = it->position;
		r.strategy = new Strategy(std::move(s));

		heap.template push<Strategy>(std::move(r));
	}

	T pop() {
		update_heap();

		while(heap.size() > 0) {
			Ref r = heap.pop();

			pheet_assert(r.strategy != nullptr);
			delete r.strategy();

			if(r.item->position == r.position) {
				if(SIZET_CAS(&(r.item->position), r.position, r.position + 1)) {
					return r.item->data;
				}
			}
			update_heap();
		}

		// Heap is empty. Try getting random item directly from fifo queue (without heap)

		// Tries to find and take a random item from the queue inside the block
		// Synchronization and verification all take place inside this method.
		// Returned item is free to use by this thread
		Item* item = head_block->take_rand_filled(head);
		if(item != nullptr) {
			return item->data;
		}

		return nullable_traits<T>::null_value;
	}

	template <class Strategy>
	void item_push(Item* item) {
		Ref r;
		r.item = item;
		r.position = item.position;
		// TODO: check whether the position is still valid, otherwise end function

		Strategy* s = new Strategy(*reinterpret_cast<Strategy*>(strategy));
		s->rebase();
		r.strategy = s;

		heap.template push<Strategy>(std::move(r));
	}
private:
	void update_heap() {
		// Check whether update is necessary
		if(!heap.empty()) {
			if(LocalKPrio) {
				size_t pos = heap.peek().position;
				ptrdiff_t diff = ((ptrdiff_t)head) - ((ptrdiff_t) pos);
				if(diff >= 0) {
					return;
				}
			}
			else {
				if(task_storage->tail == head) {
					return;
				}
			}
		}

		process_until(task_storage->head);
		while(head != task_storage->tail) {
			if(!heap.empty() && LocalKPrio) {
				size_t pos = heap.peek().position;
				ptrdiff_t diff = ((ptrdiff_t)head) - ((ptrdiff_t) pos);
				if(diff >= 0) {
					return;
				}
			}
			// If we fail to update, some other thread must have succeeded
			if(task_storage->head == head) {
				SIZET_CAS(&(task_storage->head), head, task_storage->tail);
			}
			process_until(task_storage->head);
		}
	}

	void process_until(size_t limit) {
		while(head != task_storage->head) {
			while(!head_block->in_block(head)) {
				pheet_assert(head_block->get_next() != nullptr);
				DataBlock* next = head_block->get_next();
				head_block->deregister();
				head_block = next;
			}

			Item* item = head_block->get_item(head);
			if(item->position == head) {
				// TODO: skip locally created items (maybe first check with a performance counter how often this occurs)

				// Push item to local heap
				auto ip = item->item_push;
				(this->*ip)(item);
			}

			++head;
		}
	}

	TaskStorage* task_storage;
	StrategyHeap heap;

	DataBlock* tail_block;
	DataBlock* head_block;
	size_t head;

	ItemMemoryManager items;
	DataBlockMemoryManager data_blocks;
};

} /* namespace pheet */
#endif /* CENTRALKSTRATEGYTASKSTORAGEPLACE_H_ */
