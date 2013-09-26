/*
 * KLSMLocalityTaskStoragePlace.h
 *
 *  Created on: Sep 18, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGEPLACE_H_
#define KLSMLOCALITYTASKSTORAGEPLACE_H_

#include "KLSMLocalityTaskStorageItem.h"
#include "KLSMLocalityTaskStorageBlock.h"
#include "KLSMLocalityTaskStorageFrame.h"

#include <pheet/memory/BlockItemReuse/BlockItemReuseMemoryManager.h>
#include <pheet/memory/ItemReuse/ItemReuseMemoryManager.h>

#include <limits>

namespace pheet {

template <class Pheet, class TaskStorage, class ParentTaskStoragePlace, class Strategy>
class KLSMLocalityTaskStoragePlace : public ParentTaskStoragePlace::BaseTaskStoragePlace {
public:
	typedef KLSMLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy> Self;

	typedef typename ParentTaskStoragePlace::BaseTaskStoragePlace BaseTaskStoragePlace;
	typedef KLSMLocalityTaskStorageFrame<Pheet> Frame;
	typedef typename ParentTaskStoragePlace::BaseItem BaseItem;
	typedef KLSMLocalityTaskStorageItem<Pheet, Self, Frame, BaseItem, Strategy> Item;
	typedef KLSMLocalityTaskStorageBlock<Pheet, Item> Block;

	typedef typename BaseItem::T T;

	typedef BlockItemReuseMemoryManager<Pheet, Item, KLSMLocalityTaskStorageItemReuseCheck<Item> > ItemMemoryManager;
	typedef ItemReuseMemoryManager<Pheet, Frame, KLSMLocalityTaskStorageFrameReuseCheck<Frame> > FrameMemoryManager;

	typedef Strategy2BaseTaskStorageBoundaryState BoundaryState;

	KLSMLocalityTaskStoragePlace(ParentTaskStoragePlace* parent_place)
	:parent_place(parent_place), current_frame(&(frame_memory_manager.acquire_item())), missed_tasks(0) {

		// Get central task storage at end of initialization (not before,
		// since this place becomes visible as soon as we retrieve it)
		task_storage = TaskStorage::get(this, parent_place->get_central_task_storage(), created_task_storage);

		// Fill in blocks usable for storing a single item
		// Might make sense to precreate more, but for now let's leave it at that
		blocks.push_back(new Block(1));
		blocks.push_back(new Block(1));

		top_block.store(blocks[0], std::memory_order_relaxed);
	}
	~KLSMLocalityTaskStoragePlace() {
		for(auto i = blocks.begin(); i != blocks.end(); ++i) {
			delete *i;
		}

		if(created_task_storage) {
			delete task_storage;
		}
	}

	void push(Strategy&& strategy, T data) {
		Item& it = items.acquire_item();

		it.strategy = std::move(strategy);
		it.used_locally = true;
		it.data = data;
		it.taken.store(false, std::memory_order_relaxed);
		it.task_storage = task_storage;
		if(!current_frame->low_congestion()) {
			// Exchange current frame every time there is congestion
			current_frame = &(frame_memory_manager.acquire_item());
		}
		it.frame.store(current_frame, std::memory_order_relaxed);
		it.last_phase.store(-1, std::memory_order_relaxed);

		Block* tb = top_block.load(std::memory_order_relaxed);
		if(!tb.try_put(&it)) {
			while(tb->needs_merge()) {
				Block* next = tb->get_next();

				Block* merged = find_free_block(tb->get_level() + 1);
				merged->merge_into(next, tb);

				merged->set_next(next->get_next());

				// No ordering, we don't give any guarantees on things other threads see in a local thread
				// (Of course for the global list we need to synchronize more)
				top_block.store(merged, std::memory_order_relaxed);
				tb->reset();
				tb = merged;

				next->reset();
			}

			size_t l = tb->get_level();
			pheet_assert(blocks.size() > ((l << 1) + 1));
			Block* new_tb = blocks[l << 1];
			if(!new_tb->empty()) {
				new_tb = blocks[(l << 1) + 1];
				pheet_assert(new_tb->empty());
			}
			new_tb->set_next(tb);

			// No need to order, will be released by put
			top_block.store(new_tb, std::memory_order_relaxed);

			// We know we have space in here, so no try_put needed
			new_tb.put(&it);
/*
			if(best_block == nullptr || (best_block != new_tb) {
				top = &it;
			}*/
		}

		parent_place->push(&it);
	}

	T pop(BaseItem* boundary) {
		Item* item = reinterpret_cast<Item*>(boundary);

		// Check whether boundary item is still available
		if(item->last_phase.load(std::memory_order_acquire) == -1) {

			// We can assume that boundary is stored in this task storage, so we just get the best item
			Block* best = find_best_block();

			Item* best_item = best->top();

			T data = item->take();
			// There is at least one taken task now, let's do a cleanup
			best->pop_taken_and_dead();


			T data = item->data;
			bool taken = false;
			if(item->taken.compare_exchange_strong(taken, true, std::memory_order_relaxed, std::memory_order_relaxed)) {
				return data;
			}
		}

		// Linearized to check of boundary item
		return nullable_traits<T>::null_value;
	}

	T steal(BaseItem* boundary) {
		Item* item = reinterpret_cast<Item*>(boundary);
		T data = item->data;
		bool taken = false;
		if(item->taken.compare_exchange_strong(taken, true, std::memory_order_relaxed, std::memory_order_relaxed)) {
			return data;
		}
		return nullable_traits<T>::null_value;
	}

	/*
	 * Override from parent
	 */
	void clean_up() {

	}

private:
	Block* merge(Block* block) {

	}

	Block* find_best_block() {
		Block* b = top_block.load(std::memory_order_relaxed);
		while(tb->needs_merge()) {
			Block* next = tb->get_next();

			Block* merged = find_free_block(tb->get_level() + 1);
			merged->merge_into(next, tb);

			merged->set_next(next->get_next());

			top_block.store(merged, std::memory_order_release);
			tb->reset();
			tb = merged;

			next->reset();
		}

		while(b->get_next() != nullptr && b->empty()) {
			b = b->get_next();
			top_block.store(b, std::memory_order_release);
		}
		if(b->empty()) {
			return nullptr;
		}

		Block* best = nullptr;
		prev = nullptr;
		while(b != nullptr) {
			if(b->needs_merge()) {
				Block* next = b->get_next();

				Block* merged = find_free_block(b->get_level() + 1);
				merged->merge_into(next, b);

				merged->set_next(next->get_next());

				if(prev == nullptr) {
					top_block.store(merged, std::memory_order_release);
				}
				else {

				}
				b->reset();
				b = merged;

				next->reset();
			}
			if(!b->empty()) {
				if(best == nullptr || b->top()->strategy.prioritize(best->top()->strategy)) {
					best = next;
				}
			}
			prev = b;
			b = b->get_next();
		}
		return best;
	}

	Block* find_free_block(size_t level) {
		size_t offset = level << 1;

		while(!blocks[offset].empty()) {
			++offset;
			if(blocks.size() == offset) {
				blocks.push_back(new Block(offset >> 1));
				blocks.push_back(new Block(offset >> 1));
			}
			if(blocks[offset].empty()) {
				return blocks[offset];
			}
			++offset;
		}
		return blocks[offset];
	}

	ParentTaskStoragePlace* parent_place;
	TaskStorage* task_storage;
	bool created_task_storage;

	ItemMemoryManager items;
	FrameMemoryManager frames;

	std::vector<Block*> blocks;
	std::atomic<Block*> top_block;

	Frame* current_frame;

//	Block* best_block;

	size_t missed_tasks;
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEPLACE_H_ */
