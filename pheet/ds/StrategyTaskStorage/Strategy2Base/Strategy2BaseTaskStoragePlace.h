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
#include "Strategy2BaseTaskStorageBase.h"

#include <pheet/sched/Strategy2/StrategyScheduler2BaseStrategy.h>
#include <pheet/memory/BlockItemReuse/BlockItemReuseMemoryManager.h>

#include <limits>

namespace pheet {

template <class Pheet, class TaskStorage, typename TT, size_t BlockSize>
class Strategy2BaseTaskStoragePlace : public Strategy2BaseTaskStorageBasePlace<Pheet> {
public:
	typedef Strategy2BaseTaskStoragePlace<Pheet, TaskStorage, TT, BlockSize> Self;

	typedef TT T;

	typedef typename TaskStorage::BaseTaskStorage BaseTaskStorage;
	typedef Strategy2BaseTaskStorageBasePlace<Pheet> BaseTaskStoragePlace;

	typedef Strategy2BaseTaskStorageDataBlock<Pheet, Self, BaseTaskStorage, TT, BlockSize> DataBlock;
	typedef Strategy2BaseTaskStorageBaseItem<Pheet, BaseTaskStorage, TT> BaseItem;
	typedef Strategy2BaseTaskStorageItem<Pheet, BaseTaskStorage, DataBlock, TT> Item;

	typedef Strategy2BaseTaskStoragePerformanceCounters<Pheet>
			PerformanceCounters;

	typedef BlockItemReuseMemoryManager<Pheet, Item, Strategy2BaseTaskStorageItemReuseCheck<Item> > ItemMemoryManager;
	typedef BlockItemReuseMemoryManager<Pheet, DataBlock, Strategy2BaseTaskStorageDataBlockReuseCheck<DataBlock> > DataBlockMemoryManager;

	typedef StrategyScheduler2BaseStrategy<Pheet> BaseStrategy;

	typedef typename Pheet::Scheduler::Place SchedulerPlace;

	typedef Strategy2BaseTaskStorageBoundaryState BoundaryState;

	Strategy2BaseTaskStoragePlace(TaskStorage* task_storage, SchedulerPlace* scheduler_place, PerformanceCounters pc)
	:pc(pc), task_storage(task_storage),
	 bottom(0), top(0),
	 last_partner(nullptr),
	 scheduler_place(scheduler_place)  {
		bottom_block = &(data_blocks.acquire_item());
		bottom_block->link(nullptr);
		top_block.store(bottom_block, std::memory_order_relaxed);
	//	pc.num_blocks_created.incr();

		// Full synchronization by scheduler before place becomes visible, happens-before for all initialization is satisfied
	}

	/**
	 * Only here for the compiler. Should never be called
	 */
	Strategy2BaseTaskStoragePlace(Self*) {
		throw -1;
	}

	~Strategy2BaseTaskStoragePlace() {}

	void push(T data) {
		Item& it = items.acquire_item();

		size_t b = bottom.load(std::memory_order_relaxed);

//		it.strategy_v = std::move(s);
	//	it.place = this;
		it.data = data;
		it.taken.store(false, std::memory_order_relaxed);
		it.task_storage = task_storage;
	//	it.item_push = &Self::template item_push<Strategy>;
	//	it.block = local_tail;
	//	it.offset = b;

		// If item does not fit in existing block, add a new block
		if(!bottom_block->fits(b)) {
			// First check if we already created a next block
			DataBlock* next = bottom_block->get_next();
			if(next == nullptr) {
				// Create one if not
				next = &(data_blocks.acquire_item());
				next->link(bottom_block);
			}
			bottom_block = next;
		}

		// Put item in block at position b
		bottom_block->put(&it, b);

		// If the new value of bottom is visible, the put operation must have happened before
		bottom.store(b + 1, std::memory_order_release);
	}

	void push(BaseItem* item) {
		size_t b = bottom.load(std::memory_order_relaxed);

		// If item does not fit in existing block, add a new block
		if(!bottom_block->fits(b)) {
			// First check if we already created a next block
			DataBlock* next = bottom_block->get_next();
			if(next == nullptr) {
				// Create one if not
				next = &(data_blocks.acquire_item());
				next->link(bottom_block);
			}
			bottom_block = next;
		}

		++(item->local_ref_count);

		// Put item in block at position b
		bottom_block->put(item, b);

		// If the new value of bottom is visible, the put operation must have happened before
		bottom.store(b + 1, std::memory_order_release);
	}

	/*
	 * Pops tasks in LIFO order. Tasks stored in a specialized task storage may allow
	 * other tasks with higher priority to be executed before them, thereby violating the
	 * LIFO order.
	 */
	T pop() {
		size_t b = bottom.load(std::memory_order_relaxed);
		size_t t = top.load(std::memory_order_relaxed);
		pheet_assert(((ptrdiff_t)(b - t)) >= 0);
		DataBlock* db = bottom_block;

		BaseItem* ret;
		while(true) {
			if(b == t) {
				// Empty, first make sure top and bottom are equal, then try stealing
				b = bottom.load(std::memory_order_relaxed);
				// If we saw some taken tasks on the way, b might be smaller than bottom.
				// Since top might have been updated in the meantime we cannot just store
				// b into bottom or top might overtake bottom
				// Instead we update top to the old value of bottom
				while(t != b) {
					// Make sure top has not overtaken bottom
					pheet_assert(((ptrdiff_t)(b - t)) > 0);
					top.compare_exchange_weak(t, b, std::memory_order_relaxed);
				}

				return steal();
			}
			--b;

			// Did we empty current block? If yes go to predecessor
			if(!db->fits(b)) {
				db = db->get_prev();
				pheet_assert(db != nullptr);
				pheet_assert(db->fits(b));
			}

			ret = db->get(b);
			// Has item been taken? (Due to stealing or due to other execution orders for tasks
			// with different strategies) If yes, skip task and continue
			if(ret->taken.load(std::memory_order_relaxed)) {
				continue;
			}

			// Check whether task is managed by other task storage
			if(ret->task_storage != task_storage) {
				size_t b2 = bottom.load(std::memory_order_relaxed);
				// Make sure bottom is corrected before trying to pop task
				if(b2 != b) {
					// If task_storage->pop has at least one release statement on success,
					// this statement can be relaxed completely
					// Proof: write of new value for bottom happens before acquire of taken item
					// If item was not taken, top cannot overtake bottom
					// (On failure of pop, bottom is reset, no danger there, nothing
					// to steal anyway, but some thread may change top, so bottom needs to be reset)
					bottom.store(b + 1, std::memory_order_relaxed);
				}

				// Try popping ret or some higher priority task from the sub task-storage
				// Semantics: Other task may only be popped if ret is still not taken!!!
				T ret_data = ret->task_storage->pop(ret, scheduler_place->get_id());

				if(ret_data != nullable_traits<T>::null_value) {
					// Bottom does not need to be reset, since the task we took is guaranteed
					// to have blocked progress of top pointer before we updated bottom
					bottom_block = db;
					return ret_data;
				}

				// Need to reset bottom to make sure that top cannot overtake bottom
				// (would be dangerous for push)
				// Stealing threads may still see bottom < top for a short time but this is ok
				// (will just lead to a failed steal attempt, and queue is empty in this case anyway)
				bottom.store(b2, std::memory_order_relaxed);

				continue;
			}

			break;
		}

		bottom_block = db;

		// Now we update bottom sequentially consistent
		bottom.store(b, std::memory_order_seq_cst);
		// No need to check for taken again, this item is guaranteed to be managed by this task storage,
		// therefore safety checks are based on top and bottom

		// Sequentially consistent ordering due to bottom.store(b, seq_cst) before
		t = top.load(std::memory_order_relaxed);

		ptrdiff_t diff = (ptrdiff_t)(b - t);

		if(diff > 0) {
			// As with Arora deques, due to sequential consistency of writes to top and bottom
			// it is safe to just take the item
			T ret_data = ret->data;
			pheet_assert(!ret->taken.load(std::memory_order_relaxed));
			ret->taken.store(true, std::memory_order_relaxed);
			return ret_data;
		}
		else if(diff == 0) {
			// As in Arora deque, all threads compete for the last available task
			// All will try to set top to t + 1
			bottom.store(b + 1, std::memory_order_relaxed);
			if(top.compare_exchange_strong(t, t + 1, std::memory_order_relaxed)) {
				T ret_data = ret->data;
				pheet_assert(!ret->taken.load(std::memory_order_relaxed));
				ret->taken.store(true, std::memory_order_relaxed);
				return ret_data;
			}
		}
		else {
			// Since writes to top by other threads are always sequentially consistent
			// and they are sequentially consistent with our previous write to bottom
			// top cannot have been updated since we read it after our last write to bottom
			// since top > bottom and all tasks there have already been taken before
			// so we can safely set bottom to match top
			bottom.store(t, std::memory_order_relaxed);
		}
		return steal();
	}

	bool is_full() {
		return false;
	}

	TaskStorage* get_central_task_storage() {
		return task_storage;
	}

	/**
	 * Only safe for owner of TaskStorage. For other places sometimes top > bottom may become visible
	 * leading to strange values for size.
	 */
	size_t size() {
		return bottom.load(std::memory_order_relaxed) - top.load(std::memory_order_relaxed);
	}

	void clean_up() {
		// No need for cleanup, everything is automatically cleaned up in destructor
	}
private:
	/*
	 * Tries to steal a single task from any other place. Places are selected semirandomly
	 * Steal attempts for tasks stored in different task storages will trigger a steal
	 * on that task storage. Steal does not give any guarantees on which tasks it will
	 * attempt to steal. It will usually be FIFO, but this can and will be violated for
	 * tasks that have different task storages
	 */
	T steal() {
		if(last_partner.load(std::memory_order_relaxed) != nullptr) {
			T ret = last_partner.load(std::memory_order_relaxed)->steal_from();
			if(ret != nullable_traits<T>::null_value) {
				return ret;
			}
			last_partner.store(nullptr, std::memory_order_relaxed);
		}

		procs_t num_levels = scheduler_place->get_num_levels();
		// Finalize elements in stack
		// We do not steal from the last level as there are no partners
		procs_t level = num_levels - 1;
		while(level > 0) {
			Self& partner = scheduler_place->get_random_partner_at_level(level)->get_base_task_storage();

			T ret = partner.steal_from();
			if(ret != nullable_traits<T>::null_value) {
				last_partner.store(&partner, std::memory_order_relaxed);
				return ret;
			}

			// Don't care about memory ordering, just give me some partner to check
			Self* partner_partner = partner.last_partner.load(std::memory_order_relaxed);
			if(partner_partner != nullptr) {
				ret = partner_partner->steal_from();
				if(ret != nullable_traits<T>::null_value) {
					// Found some work. Recheck this partner next time we steal
					last_partner.store(partner_partner, std::memory_order_relaxed);
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
		// The only dangerous case is that we might see b > t although
		// b = t, but acquire makes sure we see the b that the last updater to t saw.
		// b=t can only be achieved by updating t so this is ordered.
		// Sometimes we may also see b<t for a short while only if the queue is empty,
		// but b will be reset to its old value later, so this is safe
		size_t t = top.load(std::memory_order_acquire);
		size_t old_t = t;

		size_t b = bottom.load(std::memory_order_relaxed);

		ptrdiff_t diff = (ptrdiff_t)(b-t);
		// Empty check
		if(diff <= 0) {
			// Nothing to steal
			return nullable_traits<T>::null_value;
		}

		DataBlock* db = get_top_block(t);

		size_t offset;
		while(true) {
			offset = db->get_block_offset();

			ptrdiff_t diff = (ptrdiff_t)(t - offset);
			if(diff < 0) {
				// Block was already reused. To keep it wait-free, no retries
				return nullable_traits<T>::null_value;
			}
			else if(diff >= (ptrdiff_t)BlockSize) {
				db = db->get_next();
				if(db == nullptr) {
					return nullable_traits<T>::null_value;
				}
				offset = db->get_block_offset();
				continue;
			}
			break;
		}

		BaseItem* ret = db->direct_get(t - offset);

		// Skip items that have been marked as taken
		while(ret->taken.load(std::memory_order_acquire)) {
			++t;
			if(t == b) {
				// Empty now. No need to update t, other threads will do it
				// In fact, doing this by ourselves now would be dangerous
				return nullable_traits<T>::null_value;
			}

			if(t - offset >= BlockSize) {
				db = db->get_next();
				offset += BlockSize;
				if(db == nullptr || db->get_block_offset() != offset) {
					// Working on a reused block, return
					return nullable_traits<T>::null_value;
				}
			}
		}

		if(ret->task_storage != task_storage) {
			T ret_data = ret->task_storage->steal(ret, scheduler_place->get_id());

			// We were successful in stealing the given task or a task after that one.
			// So we know for sure that pop will not find any tasks that are not taken before this one
			// Therefore we can correct the top pointer to the position before ret (ret might not yet be taken)
			if(t - old_t > 16) {
				// We do not do this every time to reduce congestion
				// 16 was chosen arbitrarily and may be tuned

				size_t old_t2 = top.load(std::memory_order_relaxed);
				// Weak is enough, and if we fail it's no problem, it's just a lazy correction of top anyway
				top.compare_exchange_weak(old_t, t, std::memory_order_relaxed, std::memory_order_relaxed);
			}

			return ret_data;
		}

		T ret_data = ret->data;

		// Steal is allowed to spuriously fail so weak is enough
		if(top.compare_exchange_weak(old_t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
			pheet_assert(!ret->taken.load(std::memory_order_relaxed));
			ret->taken.store(true, std::memory_order_relaxed);
			return ret_data;
		}
		return nullable_traits<T>::null_value;
	}

	/**
	 * Corrects the top block pointer if necessary, and marks old blocks as reusable
	 * Can be called by any thread
	 * May only be called on non-empty queues, otherwise there may be a corner-case
	 * where t moved past the block, but the next block has not yet been initialized
	 * For this reason t has to be passed, so that the t is used for which the
	 * non-empty check is performed (linearized to there)
	 */
	DataBlock* get_top_block(size_t t) {
		DataBlock* db = top_block.load(std::memory_order_relaxed);

		if(!db->fits(t)) {
			// Only one thread may update the top block, we use a TASLock for that
			// Is not blocking progress of other threads, only blocking progress for cleanup
			if(!top_block_lock.test_and_set(std::memory_order_relaxed)) {
				// Reload top_block, just in case it was changed in the meantime
				db = top_block.load(std::memory_order_relaxed);

				// Update the list
				// (It is necessary to recheck if it fits before the first iteration,
				// since the top block might have changed)
				while(!db->fits(t)) {
					DataBlock* next = db->get_next();
					pheet_assert(next != nullptr);
					top_block.store(next, std::memory_order_relaxed);
					// Has release semantics, therefore making the update to top_block visible as well
					db->mark_reusable();
					db = next;
				}
				// Free lock, so other threads can update top_block now
				top_block_lock.clear(std::memory_order_release);

				return db;
			}
			else {
				// Let some other thread do the clean-up and just iterate through the blocks
				return db;
			}
		}
		return db;
	}

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
	std::atomic_flag top_block_lock;

	std::atomic<Self*> last_partner;

	SchedulerPlace* scheduler_place;
};

} /* namespace pheet */
#endif /* STRATEGY2BASETASKSTORAGEPLACE_H_ */
