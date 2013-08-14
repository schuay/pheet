/*
 * Strategy2BaseTaskStoragePlace.h
 *
 *  Created on: 12.08.2013
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef STRATEGY2BASETASKSTORAGEPLACE_H_
#define STRATEGY2BASETASKSTORAGEPLACE_H_

#include "Strategy2BaseTaskStorageDataBlock.h"
#include "Strategy2BaseTaskStorageItem.h"
#include "Strategy2BaseTaskStoragePerformanceCounters.h"

#include <pheet/sched/Strategy2/StrategyScheduler2BaseStrategy.h>
#include <pheet/memory/BlockItemReuse/BlockItemReuseMemoryManager.h>

#include <limits>

namespace pheet {

template <class Pheet, class TaskStorage, typename TT, size_t BlockSize>
class Strategy2BaseTaskStoragePlace {
public:
	typedef Strategy2BaseTaskStoragePlace<Pheet, TaskStorage, TT, BlockSize> Self;

	typedef TT T;

	typedef Strategy2BaseTaskStorageDataBlock<Pheet, Self, TT, BlockSize> DataBlock;
	typedef Strategy2BaseTaskStorageBaseItem<Pheet, TT> BaseItem;
	typedef Strategy2BaseTaskStorageItem<Pheet, DataBlock, TT> Item;

	typedef Strategy2BaseTaskStoragePerformanceCounters<Pheet>
			PerformanceCounters;

	typedef BlockItemReuseMemoryManager<Pheet, Item, Strategy2BaseTaskStorageItemReuseCheck<Item> > ItemMemoryManager;
	typedef BlockItemReuseMemoryManager<Pheet, DataBlock, Strategy2BaseTaskStorageDataBlockReuseCheck<DataBlock> > DataBlockMemoryManager;

	typedef StrategyScheduler2BaseStrategy<Pheet> BaseStrategy;

	typedef typename Pheet::Scheduler::Place SchedulerPlace;

	Strategy2BaseTaskStoragePlace(TaskStorage* task_storage, SchedulerPlace* scheduler_place, PerformanceCounters pc)
	:pc(pc), task_storage(task_storage),
	 bottom(0), top(0), spy_bottom(0), spy_top(0),
	 scheduler_place(scheduler_place)  {
		bottom_block = &(data_blocks.acquire_item());
		bottom_block->link(nullptr);
		top_block->store(bottom_block, std::memory_order_relaxed);
		pc.num_blocks_created.incr();

		// Full synchronization by scheduler before place becomes visible, happens-before for all initialization is satisfied
	}

	~Strategy2BaseTaskStoragePlace() {}

	void push(BaseStrategy&& s, T data) {
		Item& it = items.acquire_item();

		// Cannot check this since item may be uninitialized if it has been newly allocated
//		pheet_assert(it.strategy == nullptr);

		size_t b = bottom.load(std::memory_order_relaxed);

//		it.strategy_v = std::move(s);
		it.place = this;
		it.data = data;
		it.taken.store(false, std::memory_order_relaxed);
		it.task_storage = task_storage;
	//	it.item_push = &Self::template item_push<Strategy>;
	//	it.block = local_tail;
		it.offset = b;

		if(!bottom_block->fits(b)) {
			DataBlock* next = bottom_block->get_next();
			if(next == nullptr) {
				next = &(data_blocks.acquire_item());
				next->link(bottom_block);
			}
			bottom_block = next;
		}

		bottom_block->put(it);
		// If the new value of bottom is visible, the put operation must have happened before
		bottom.store(b + 1, std::memory_order_release);
	}

	T pop() {
		// Current problem: Tasks for other task storages can trigger linear scans through the data-structure

		size_t b = bottom.load(std::memory_order_relaxed);
		size_t t = top.load(std::memory_order_relaxed);
		DataBlock* db = bottom_block;

		BaseItem* ret;
		while(true) {
			if(b == t) {
				// Empty, first make sure top and bottom are equal, then try stealing
				b = bottom.load(std::memory_order_relaxed);
				while(t != b) {
					top.compare_exchange_weak(t, b, std::memory_order_relaxed);
				}

				return steal();
			}
			if(!db->fits(b)) {
				db = db->get_prev();
				pheet_assert(db != nullptr);
			}

			ret = db->get(b);
			if(ret->taken.load(std::memory_order_relaxed)) {
				--b;
				continue;
			}

			if(ret->task_storage != task_storage) {
				size_t b2 = bottom.load(std::memory_order_relaxed);
				if(b2 != b) {
					// If task_storage->pop has at least one release statement on success,
					// this statement can be relaxed completely
					// Otherwise it would need to be seq_cst
					// Proof: write of new value for bottom happens before acquire of taken item
					// If item was not taken, top cannot overtake bottom
					// (On failure of pop, bottom is reset, no danger there, nothing
					// to steal anyway, but some thread may change top, so bottom needs to be reset)
					bottom.store(b, std::memory_order_relaxed);
				}

				// Try popping ret or some higher priority task from the sub task-storage
				T ret_data = ret->task_storage->pop(ret, scheduler_place->get_id());
				if(ret_data != nullable_traits<T>::null_value) {
					bottom_block = db;
					return ret_data;
				}

				// Need to reset bottom to make sure that top cannot overtake bottom
				// (would be dangerous for push)
				// Stealing threads may still see bottom < top but this is ok
				// (will just lead to a failed steal attempt, and queue is empty in this case anyway)
				bottom.store(b2, std::memory_order_relaxed);

				--b;
				continue;
			}
			break;
		}
		bottom_block = db;

		--b;
		bottom.store(b, std::memory_order_seq_cst);

		t = top.load(std::memory_order_relaxed);

		ptrdiff_t diff = (ptrdiff_t)(b-t);

		if(diff > 0) {
			T ret_data = ret->data;
			ret->taken.store(true, std::memory_order_relaxed);
			return ret_data;
		}
		else if(diff == 0) {
			bottom.store(b + 1, std::memory_order_relaxed);
			if(top.compare_exchange_strong(t, t + 1, std::memory_order_relaxed)) {
				T ret_data = ret->data;
				ret->taken.store(true, std::memory_order_relaxed);
				return ret_data;
			}
		}
		else {
			pheet_assert(diff == -1);
			bottom.store(b + 1, std::memory_order_relaxed);
		}
		return steal();
	}

	bool is_full() {
		return false;
	}

	TaskStorage* get_central_task_storage() {
		return task_storage;
	}

	size_t size() {
		return bottom.load(std::memory_order_relaxed) - top.load(std::memory_order_relaxed);
	}
private:
	T steal() {
		// Problem: top_block is never updated and such blocks never freed. How can we do this
		// without ABA problem?

		if(last_partner != nullptr) {
			T ret = last_partner->steal_from();
			if(ret != nullable_traits<T>::null_value) {
				return ret;
			}
			last_partner = null;
		}

		procs_t num_levels = scheduler_place->get_num_levels();
		// Finalize elements in stack
		// We do not steal from the last level as there are no partners
		procs_t level = num_levels - 1;
		while(level > 0) {
			Self& partner = scheduler_place->get_random_partner_at_level(level)->get_task_storage();

			T ret = partner.steal_from();
			if(ret != nullable_traits<T>::null_value) {
				last_partner = &partner;
				return ret;
			}

			if(partner.last_partner != nullptr) {
				ret = partner.last_partner->steal_from();
				if(ret != nullable_traits<T>::null_value) {
					last_partner = partner.last_partner;
					return ret;
				}
			}

			--level;
		}

		return nullable_traits<T>::null_value;
	}

	/**
	 * Executed in the context of the stealer, should only do thread-safe things
	 */
	T steal_from() {
		size_t t = top.load(std::memory_order_acquire);

		size_t b = bottom.load(std::memory_order_relaxed);

		DataBlock* db = top_block.load(std::memory_order_relaxed);
		int cmp;
		while((cmp = db->compare(t)) != 0) {
			if(cmp < 0) {
				// To keep it wait-free, no retries
				return nullable_traits<T>::null_value;
			}
			else if(cmp > 0) {
				db = db->get_next();
				pheet_assert(db != nullptr);
			}
		}

		ptrdiff_t diff = (ptrdiff_t)(b-t);
		if(diff <= 0) {
			return nullable_traits<T>::null_value;
		}

		BaseItem* ret = db->get(t);
		while(ret->taken.load(std::memory_order_acquire)) {
			++t;

			if(!db->fits(t)) {
				db = db->get_next();
				if(db == nullptr || !db->fits(t)) {
					// Working on an old block, return
					return nullable_traits<T>::null_value;
				}
			}
		}

		if(ret->task_storage != task_storage) {
			return ret->task_storage->steal(ret, scheduler_place->get_id());
		}

		T ret_data = ret->data;

		if(top.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
			ret->taken->store(true, std::memory_order_relaxed);
			return ret_data;
		}
		return nullable_traits<T>::null_value;
	}

	/**
	 * Corrects the top block pointer if necessary, and marks old blocks as reusable
	 * Can be called by any thread
	 */
	void update_top_block() {
		size_t t = top.load(std::memory_order_acquire);
		DataBlock* db = top_block.load(std::memory_order_relaxed);

		int cmp;
		while((cmp = db->compare(t)) != 0) {
			if(cmp < 0) {
				// To keep it wait-free, no retries
				return;
			}
			else if(cmp > 0) {
				db = db->get_next();
				pheet_assert(db != nullptr);
			}
		}
	}


/*	{
		// All those variables are only written locally, so relaxed reading is enough
		size_t b = spy_bottom.load(std::memory_order_relaxed);
		size_t t = spy_top.load(std::memory_order_relaxed);
		DataBlock* bl = spy_top_b.load(std::memory_order_relaxed);

		if(b == t) {
			if(spy()) {
				b = spy_bottom.load(std::memory_order_relaxed);
			}
			else {
				return nullable_traits<T>::null_value;
			}
		}

		while(true) {
			if(b == t) {
				spy_top.store(t, std::memory_order_relaxed);
				return nullable_traits<T>::null_value;
			}
			if(!spy_top_b->fits(t)) {
				DataBlock* old_top = spy_top;
				spy_top_b = spy_top_b->get_next();
				pheet_assert(spy_top_b != nullptr);
				old_top->mark_reusable();
			}
			Item* ret = spy_top->get(t);

			while(ret->taken.load(std::memory_order_relaxed)) {
				++t;
				continue;
			}

			BaseItem* ret = spy_top->get(t);
			if(ret->task_storage != task_storage) {
				// Try popping ret or some higher priority task from the sub task-storage
				T ret_data = ret->task_storage->pop(ret, scheduler_place->get_id());
				if(ret_data != nullable_traits<T>::null_value) {
					return ret_data;
				}

				// No item with higher priority available, continue;
				++t;
				continue;
			}

			Item* ret2 = reinterpret_cast<Item*> ret;
			DataBlock* db = ret2->data_block;
			size_t offset = ret2->offset;

			Place* p = db->get_place();
			pheet_assert(p != this);

			size_t pt = p->top.load(std::memory_order_relaxed);
			// Any taking of tasks happened before we read bottom, so they are guaranteed to have been stolen
			// Top was read before bottom, so the value for bottom is at least as new as the value for top
			size_t pb = p->bottom.load(std::memory_order_seq_cst);

			ptrdiff_t bdiff = (ptrdiff_t)(b - offset);
			if(bdiff <= 0) {
				// Item has been processed by owner - continue
				++t;
				continue;
			}

			// We need to check whether the stored item is still the same as the spied item
			if(ret != ret2->data_block->get(offset)) {
				// Item has been updated in the meantime
				++t;
				continue;
			}

			ptrdiff_t diff = (ptrdiff_t)(offset - pt);
			if(diff > 0) {
				// Obviously we have skipped some items, correct this by updating top
				// Since we know for sure that other items have not been processed by owner
				// (otherwise the bdiff check would have failed, or the stored item would be different)
				// we can safely skip all items with one cas (but not the current item!)

				// Warning! Can only work if we know for sure that no items were skipped while spying!!!
				p->top.compare_exchange_strong(pt, offset, std::memory_order_relaxed);
				diff = (ptrdiff_t)(offset - pt);

				// If we failed, some other thread must have succeeded in fixing this
				pheet_assert(diff <= 0);
			}
			if(diff < 0) {
				// Top has already moved past offset, item just hasn't been marked taken yet, continue
				++t;
				continue;
			}

			T ret_data = ret->data;
			// TODO: finish.
		}
	}*/

	PerformanceCounters pc;

	ItemMemoryManager items;
	DataBlockMemoryManager data_blocks;

	TaskStorage* task_storage;
//	StrategyRetriever sr;

	// Arora-like top and bottom. Used for local tasks
	std::atomic<size_t> bottom;
	std::atomic<size_t> top;

	DataBlock* bottom_block;
	std::atomic<DataBlock*> top_block;

	Self* last_partner;

	SchedulerPlace* scheduler_place;
};

} /* namespace pheet */
#endif /* STRATEGY2BASETASKSTORAGEPLACE_H_ */
