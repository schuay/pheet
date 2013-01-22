/*
 * DistKStrategyTaskStoragePlace.h
 *
 *  Created on: 15.01.2013
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef DISTKSTRATEGYTASKSTORAGEPLACE_H_
#define DISTKSTRATEGYTASKSTORAGEPLACE_H_

#include "DistKStrategyTaskStorageDataBlock.h"
#include "DistKStrategyTaskStorageItem.h"
#include "DistKStrategyTaskStoragePerformanceCounters.h"

#include <pheet/memory/ItemReuse/ItemReuseMemoryManager.h>

#include <limits>

namespace pheet {

template <class Pheet, class Item>
struct DistKStrategyTaskStorageItemReference {
	Item* item;
	size_t position;
	// If strategy equals item->strategy => locally created
	typename Pheet::Scheduler::BaseStrategy* strategy;
};

template <class Pheet, class Ref>
class DistKStrategyTaskStorageStrategyRetriever {
public:
	typedef DistKStrategyTaskStorageStrategyRetriever<Pheet, Ref> Self;

	DistKStrategyTaskStorageStrategyRetriever() {}

	typename Pheet::Scheduler::BaseStrategy* operator()(Ref const& ref) {
		return ref.strategy;
	}

	inline bool is_active(Ref const& ref) {
		return ref.item.position == ref.position;
	}

	inline void mark_removed(Ref& ref) {
	//	item.block->mark_removed(item.index, task_storage->get_current_view());
	}

private:
};

template <class Pheet, class TaskStorage, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize, bool LocalKPrio>
class DistKStrategyTaskStoragePlace {
public:
	typedef DistKStrategyTaskStoragePlace<Pheet, TaskStorage, TT, StrategyHeapT, BlockSize, LocalKPrio> Self;

	typedef DistKStrategyTaskStorageDataBlock<Pheet, Self, TT, BlockSize> DataBlock;

	typedef TT T;
	typedef DistKStrategyTaskStorageItem<Pheet, Self, TT> Item;
	typedef DistKStrategyTaskStorageItemReference<Pheet, Item> Ref;
	typedef DistKStrategyTaskStorageStrategyRetriever<Pheet, Ref> StrategyRetriever;

	typedef StrategyHeapT<Pheet, Ref, StrategyRetriever> StrategyHeap;
	typedef DistKStrategyTaskStoragePerformanceCounters<Pheet, typename StrategyHeap::PerformanceCounters>
			PerformanceCounters;

	typedef ItemReuseMemoryManager<Pheet, Item, DistKStrategyTaskStorageItemReuseCheck<Item> > ItemMemoryManager;
	typedef ItemReuseMemoryManager<Pheet, DataBlock, DistKStrategyTaskStorageDataBlockReuseCheck<DataBlock> > DataBlockMemoryManager;

	typedef typename Pheet::Scheduler::Place SchedulerPlace;

	DistKStrategyTaskStoragePlace(TaskStorage* task_storage, SchedulerPlace* scheduler_place, PerformanceCounters pc)
	:pc(pc), task_storage(task_storage), heap(sr, pc.strategy_heap_performance_counters), remaining_k(std::numeric_limits<size_t>::max()),
	 local_head(&(data_blocks.acquire_item())), local_tail(local_head), last_steal_head(nullptr), scheduler_place(scheduler_place)  {
		local_head->reset(0);

		DataBlock* tmp = task_storage->start_block;
		if(tmp == nullptr) {
			// This assumes the first place is initialized before all others, otherwise synchronization would be needed!
			tmp = &(data_blocks.acquire_item());
			tmp->init_global_first(task_storage->get_num_places());
			task_storage->start_block = tmp;

			pc.num_blocks_created.incr();
			pc.num_global_blocks.incr();
		}
		pc.num_blocks_created.incr();
		global_tail = tmp;
	}

	~DistKStrategyTaskStoragePlace() {
		// Check whether this is needed at all, or if scheduler only terminates if heap is empty
		while(!heap.empty()) {
			Ref r = heap.pop();
			// All items should have been processed
			pheet_assert(r.position != r.item->position);
			// Is potentially dangerous. Maybe item has already been deleted? Check...
			if(r.strategy != r.item->strategy) {
				delete r.strategy;
			}
			r.strategy = nullptr;
		}
	}

	template <class Strategy>
	void push(Strategy&& s, T data) {
		Item& it = items.acquire_item();
		pheet_assert(it.strategy == nullptr);
		size_t k = s.get_k();
		pheet_assert(k > 0);

		it.strategy = new Strategy(std::move(s));
		it.data = data;
		it.item_push = &Self::template item_push<Strategy>;

		if(k < remaining_k) remaining_k = k;
		local_tail->put(&it);

		pheet_assert(remaining_k != 0);

		--remaining_k;
		if(remaining_k == 0) {
			make_queue_global();
		}
		pheet_assert(remaining_k != 0);

		if(local_tail->is_full()) {
			pc.num_blocks_created.incr();

			DataBlock* next = &(data_blocks.acquire_item());
			local_tail->set_next(next);
			local_tail = next;

			last_steal_head = nullptr;
		}
		pheet_assert(local_tail->get_filled() < BlockSize);

		Ref r;
		r.item = &it;
		r.position = it.orig_position;
		r.strategy = it.strategy;

		heap.template push<Strategy>(std::move(r));
	}

	T pop() {
		do {
			update_heap();

			while(heap.size() > 0) {
				Ref r = heap.pop();

				pheet_assert(r.strategy != nullptr);
				if(r.strategy != r.item->strategy) {
					delete r.strategy;
				}

				if(r.item->position == r.position) {
					T ret = r.item->data;
					if(SIZET_CAS(&(r.item->position), r.position, r.position + 1)) {
						pc.num_successful_takes.incr();
						while(local_head != local_tail && local_head->is_empty()) {
							local_head = local_head->get_next();
						}

						return ret;
					}
					else {
						pc.num_unsuccessful_takes.incr();
					}
				}
				update_heap();
			}

		// Heap is empty. Try work-stealing (without actual stealing, just copying)
		} while(steal_work());
		while(local_head != local_tail && local_head->is_empty()) {
			local_head = local_head->get_next();
		}

		return nullable_traits<T>::null_value;
	}

	template <class Strategy>
	void item_push(Item* item, size_t position) {
		Ref r;
		r.item = item;
		r.position = position;
		// TODO: check whether the position is still valid, otherwise end function

		Strategy* s = new Strategy(*reinterpret_cast<Strategy*>(item->strategy));
		r.strategy = s;

		heap.template push<Strategy>(std::move(r));
	}

	bool is_full() {
		return false;
	}

	TaskStorage* get_central_task_storage() {
		return task_storage;
	}

	size_t size() {
		return heap.size();
	}
private:
	void update_heap() {
		// Check whether update is necessary
		if(!heap.empty()) {
			if(LocalKPrio) {

			}
			else {
				process_global_queue();
			}
		}
		else {
			process_global_queue();
		}
	}

	void make_queue_global() {
		if(task_storage->get_num_places() > 1) {
			pc.num_blocks_created.incr();
			DataBlock* new_list = &(data_blocks.acquire_item());
			new_list->reset(local_tail->get_offset() + BlockSize);

			size_t np = task_storage->get_num_places() - 1;
			DataBlock* block = local_head;
			while(block != nullptr) {
				pc.num_global_blocks.incr();
				block->set_active_threads(np);
//				block->target_active_threads = 0;
				block = block->get_next();
			}
			local_tail->set_active_threads(np + 1);

			do {
				process_global_queue();

			} while(!global_tail->connect_list(local_head));

			global_tail->mark_processed_globally();
			global_tail = local_tail;

			block = local_head;
			while(block != nullptr) {
				block->set_state(2);
				block = block->get_next();
			}
			local_head = new_list;
			local_tail = new_list;
		}
		remaining_k = std::numeric_limits<size_t>::max();
	}

	void process_global_queue() {
		DataBlock* next = global_tail->get_next();
		while(next != nullptr) {
			global_tail->mark_processed_globally();

			for(size_t i = 0; i < next->get_filled(); ++i) {
				Item* data = next->get_item(i);
				if(data->position == next->get_offset() + i) {
					// Item is usable
					auto ip = data->item_push;
					(this->*ip)(data, next->get_offset() + i);
				}
			}

			global_tail = next;
			next = next->get_next();
		}
	}

	bool steal_work() {
		procs_t num_levels = scheduler_place->get_num_levels();
		// Finalize elements in stack
		// We do not steal from the last level as there are no partners
		procs_t level = num_levels - 1;
		while(level > 0) {
			Self& partner = scheduler_place->get_random_partner_at_level(level)->get_task_storage();

			if(process_local_list(partner.local_head)) {
				// Success when stealing!
				last_steal_head = partner.local_head;
				return true;
			}
			else if(process_local_list(partner.last_steal_head)) {
				last_steal_head = partner.last_steal_head;
				return true;
			}

			--level;
		}

		return false;
	}

	bool process_local_list(DataBlock* block) {
		bool success = false;

		while(block != nullptr) {
			if(block->get_state() != 0) {
				break;
			}
			for(size_t i = 0; i < block->get_filled(); ++i) {
				Item* data = block->get_item(i);
				if(data->position == block->get_offset() + i) {

					block->register_locally();

					// Item has to be rechecked after registration (Might have been invalidated in the meantime)
					for(size_t j = i; j < block->get_filled(); ++j) {
						Item* data = block->get_item(j);
						if(data->position == block->get_offset() + j) {
							success = true;

							// Item is usable
							auto ip = data->item_push;
							(this->*ip)(data, block->get_offset() + j);
						}
					}
					block->deregister_locally();
					break;
				}
			}

			block = block->get_next();
		}

		if(success && LocalKPrio) {
			// Enforce global queue in this case. With non-local it will be called anyway later
			process_global_queue();
		}
		return success;
	}

	PerformanceCounters pc;

	ItemMemoryManager items;
	DataBlockMemoryManager data_blocks;

	TaskStorage* task_storage;
	StrategyRetriever sr;
	StrategyHeap heap;

	size_t remaining_k;
	DataBlock* local_head;
	DataBlock* local_tail;

	DataBlock* global_tail;

	DataBlock* last_steal_head;

	SchedulerPlace* scheduler_place;
};

} /* namespace pheet */
#endif /* DISTKSTRATEGYTASKSTORAGEPLACE_H_ */
