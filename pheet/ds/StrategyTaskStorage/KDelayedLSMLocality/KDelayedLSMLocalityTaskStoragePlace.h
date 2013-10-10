/*
 * KDelayedLSMLocalityTaskStoragePlace.h
 *
 *  Created on: Sep 18, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KDELAYEDLSMLOCALITYTASKSTORAGEPLACE_H_
#define KDELAYEDLSMLOCALITYTASKSTORAGEPLACE_H_

#include "KDelayedLSMLocalityTaskStorageItem.h"
#include "KDelayedLSMLocalityTaskStorageBlock.h"
#include "KDelayedLSMLocalityTaskStorageFrame.h"
#include "KDelayedLSMLocalityTaskStorageGlobalListItem.h"

#include <pheet/memory/BlockItemReuse/BlockItemReuseMemoryManager.h>
#include <pheet/memory/ItemReuse/ItemReuseMemoryManager.h>

#include <limits>
#include <unordered_map>

namespace pheet {

template <class Pheet, class TaskStorage, class ParentTaskStoragePlace, class Strategy>
class KDelayedLSMLocalityTaskStoragePlace : public ParentTaskStoragePlace::BaseTaskStoragePlace {
public:
	typedef KDelayedLSMLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy> Self;

	typedef typename ParentTaskStoragePlace::BaseTaskStoragePlace BaseTaskStoragePlace;
	typedef KDelayedLSMLocalityTaskStorageFrame<Pheet> Frame;
	typedef KDelayedLSMLocalityTaskStorageFrameRegistration<Pheet, Frame> FrameReg;
	typedef typename ParentTaskStoragePlace::BaseItem BaseItem;
	typedef KDelayedLSMLocalityTaskStorageItem<Pheet, Self, Frame, FrameReg, BaseItem, Strategy> Item;
	typedef KDelayedLSMLocalityTaskStorageBlock<Pheet, Item> Block;
	typedef KDelayedLSMLocalityTaskStorageGlobalListItem<Pheet, Block> GlobalListItem;

	typedef typename BaseItem::T T;

	typedef BlockItemReuseMemoryManager<Pheet, Item, KDelayedLSMLocalityTaskStorageItemReuseCheck<Item, Frame> > ItemMemoryManager;
	typedef ItemReuseMemoryManager<Pheet, Frame, KDelayedLSMLocalityTaskStorageFrameReuseCheck<Frame> > FrameMemoryManager;
	typedef ItemReuseMemoryManager<Pheet, GlobalListItem, KDelayedLSMLocalityTaskStorageGlobalListItemReuseCheck<GlobalListItem> > GlobalListItemMemoryManager;

	typedef typename ParentTaskStoragePlace::PerformanceCounters PerformanceCounters;

	KDelayedLSMLocalityTaskStoragePlace(ParentTaskStoragePlace* parent_place)
	:pc(parent_place->pc),
	 parent_place(parent_place), current_frame(&(frames.acquire_item())),
	 remaining_k(std::numeric_limits<size_t>::max()), tasks_popped_since_sync(0),
	 tasks(0), last_item(0), last_sync_item(0) {

		// Get central task storage at end of initialization (not before,
		// since this place becomes visible as soon as we retrieve it)
		task_storage = TaskStorage::get(this, parent_place->get_central_task_storage(), created_task_storage);

		global_list = task_storage->get_global_list_head();

		// Fill in blocks usable for storing a single item
		// Might make sense to precreate more, but for now let's leave it at that
		blocks.push_back(new Block(1));
		pc.num_blocks_created.incr();
		blocks.push_back(new Block(1));
		pc.num_blocks_created.incr();
		blocks.push_back(new Block(1));
		pc.num_blocks_created.incr();

		top_block.store(blocks[0], std::memory_order_relaxed);
		bottom_block = blocks[0];
		bottom_block->mark_in_use();
	}

	~KDelayedLSMLocalityTaskStoragePlace() {
		for(auto i = blocks.begin(); i != blocks.end(); ++i) {
			delete *i;
		}

		if(created_task_storage) {
			delete task_storage;
		}
	}

	GlobalListItem* create_global_list_item() {
		GlobalListItem* ret = &(global_list_items.acquire_item());
		ret->reset();
		return ret;
	}

	void push(Strategy&& strategy, T data) {
		Item& it = items.acquire_item();

		it.strategy = std::move(strategy);
		pheet_assert(!it.used_locally);
		it.used_locally = true;
		it.data = data;
		it.task_storage = task_storage;
		it.owner = this;
		if(!current_frame->medium_contention()) {
			// Exchange current frame every time there is congestion
			current_frame = &(frames.acquire_item());
		}
		it.frame.store(current_frame, std::memory_order_release);

		// Release, so that if item is seen as not taken by other threads
		// It is guaranteed to be newly initialized and the strategy set.
		it.taken.store(false, std::memory_order_release);
		it.last_phase.store(-1, std::memory_order_release);
		it.global = false;
		it.id = ++last_item;

		put(&it);

		parent_place->push(&it);

		// Bookkeeping for k-relaxation
		size_t k = it.strategy.get_k();
		bottom_block->added_local_item(k);
		remaining_k = std::min(remaining_k - 1, bottom_block->get_k());
		if(remaining_k == 0) {
			add_to_global_list();
		}
	}

	T pop(BaseItem* boundary) {
		Item* item = reinterpret_cast<Item*>(boundary);

		// Check whether boundary item is still available
		while(!item->is_taken()) {
			// We can assume that boundary is stored in this task storage, so we just get the best item
			Block* best = find_best_block();

			if(best == nullptr || item->is_taken()) {
				return nullable_traits<T>::null_value;
			}

			Item* best_item = best->top();
			if(best_item->strategy.get_k() <= tasks_popped_since_sync ||
					(best_item->owner == this && ((ptrdiff_t)(last_sync_item - best_item->id)) < 0)) {
				process_global_list();
				continue;
			}

			// Take does not do any deregistration, this is done by pop_taken_and_dead
			T data = best_item->take();

			// There is at least one taken task now, let's do a cleanup
			tasks_popped_since_sync += best->pop_taken_and_dead(this, frame_regs);

			// Check whether we succeeded
			if(data != nullable_traits<T>::null_value) {
				tasks.store(tasks.load(std::memory_order_relaxed) - 1, std::memory_order_relaxed);
				return data;
			}
		}

		// Linearized to check of boundary item
		return nullable_traits<T>::null_value;
	}

	T steal(BaseItem* boundary) {
		// TODO: make sure that items are not reread every time we steal. Could lead to bad scalability with large k if steal order coincides with FIFO order
		Item* item = reinterpret_cast<Item*>(boundary);

		// Should not be possible. Owner must have observed taken == false and will never go into steal
		// All owned items must be taken, otherwise no steal would be required
		// If it does, problems might occur with the reference counting
		pheet_assert(item->owner != this);

		// No need to do cheap taken check before, was already done by parent
		Frame* f = item->frame.load(std::memory_order_relaxed);
		FrameReg& reg = frame_regs[f];

		if(!reg.try_add_ref(f)) {
			// Spurious failure
			return nullable_traits<T>::null_value;
		}
		if(f == item->frame.load(std::memory_order_relaxed)) {
			if(!item->is_taken()) {
				// Boundary item is still active. Put it in local queue, so we don't have to steal it another time
				// For the boundary item we ignore whether the item is dead, as long as it is not taken, it is
				// valid to be used
				put(item);

				// Parent place must know about the item as well, to omit multiple steals
				// (unless boundary has highest priority and does not create new tasks, then
				// stealing is inevitable for correct semantics)
				parent_place->push(item);

				// Only go through tasks of other thread if we have significantly less tasks stored locally
				// This is mainly to reduce spying in case the boundary items coincide with the
				// highest priority items, and k is large, where up to k items are looked at
				// every time an item is stolen.
				if(tasks.load(std::memory_order_relaxed) < (item->owner->tasks.load(std::memory_order_relaxed) << 1)) {
					// Now go through other tasks in the blocks of the given thread.
					// There is no guarantee that all tasks will be seen, since none of these tasks
					// would violate the k-requirements, this is not an issue
					// We call this spying
					Block* b = item->owner->top_block.load(std::memory_order_acquire);
					while(b != nullptr) {
						size_t filled = b->acquire_filled();
						for(size_t i = 0; i < filled; ++i) {
							Item* spy_item = b->get_item(i);
							// First do cheap taken check
							if(spy_item != item && !spy_item->taken.load(std::memory_order_acquire)) {
								// Should never happen, we must have observed taken being set or we wouldn't
								// need to spy
								pheet_assert(spy_item->owner != this);
								Frame* spy_frame = spy_item->frame;
								FrameReg* spy_reg = &(frame_regs[spy_frame]);

								// We do not need to see all items, so only proceed if we succeed
								// registering for the frame
								if(spy_reg->try_add_ref(spy_frame)) {

									// Frame might have changed between read and registration
									Frame* spy_frame2 = spy_item->frame;
									if(spy_frame2 == spy_frame) {
										// Frame is still the same, we can proceed

										if(spy_item->is_taken_or_dead()) {
											// We do not keep items that are already taken or marked as dead
											spy_reg->rem_ref(spy_frame);
										}
										else {
											// Store item, but do not store in parent! (It's not a boundary after all!)
											put(spy_item);
										}
									}
									else {
										// Frame has changed, just skip item
										spy_reg->rem_ref(spy_frame);

										// If item has changed, so has the block, so we can skip
										// the rest of the block
										break;
									}
								}
							}
						}

						// Not wait-free yet, since theoretically we might loop through blocks
						// of same size all the time (unrealistic, but not impossible)
						// Still, not really a problem, since it only depends on the behaviour of
						// the owner, and whp. we will not encounter more than a few cycles
						// before we reach a smaller block (we can never reach a larger block)
						b = b->acquire_next();
					}
				}

				// Now that we have spied some items from the owner of the boundary, we can just pop an item
				return pop(boundary);
			}
			else {
				// Help other thread marking boundary item as taken
				// Safe since we are still registered to frame so it cannot be reused
				// (same with the thread we are helping)
				item->taken.store(true, std::memory_order_release);

				// Now we can deregister from boundary item
				reg.rem_ref(f);
			}
		}
		else {
			// Frame has changed in the meantime, just deregister
			reg.rem_ref(f);
		}

		return nullable_traits<T>::null_value;
	}

	/*
	 * Override from parent
	 */
	void clean_up() {

	}

	PerformanceCounters pc;
private:
	void process_global_list() {
		GlobalListItem* next = global_list->move_next();
		while(next != nullptr) {
			global_list = next;

			Block* prev = nullptr;
			Block* b = global_list->acquire_block();
			// We might have to do this multiple times, since a block might be reused in the meantime
			// If block == nullptr all tasks in block have been taken or appear in a later global block
			// So we can safely skip it then
			while(b != nullptr && prev != b) {
				size_t filled = b->acquire_owned_filled();
				for(size_t i = 0; i < filled; ++i) {
					Item* spy_item = b->get_owned_item(i);
					pc.num_inspected_global_items.incr();

					// First do cheap taken check
					if(spy_item->owner != this && !spy_item->taken.load(std::memory_order_acquire)) {
						Frame* spy_frame = spy_item->frame;
						FrameReg* spy_reg = &(frame_regs[spy_frame]);

						spy_reg->add_ref(spy_frame);

						// Frame might have changed between read and registration
						Frame* spy_frame2 = spy_item->frame;
						if(spy_frame2 == spy_frame) {
							// Frame is still the same, we can proceed

							if(spy_item->is_taken_or_dead()) {
								// We do not keep items that are already taken or marked as dead
								spy_reg->rem_ref(spy_frame);
							}
							else {
								// Store item, but do not store in parent! (It's not a boundary after all!)
								put(spy_item);
							}
						}
						else {
							// Frame has changed, just skip item
							spy_reg->rem_ref(spy_frame);

							// If item has changed, so has the block, so we can skip
							// the rest of the block. (either filled is now smaller or
							// block has been replaced)
							break;
						}
					}
				}


				prev = b;
				// Reload block, to see if it changed
				b = global_list->acquire_block();
			}

			next = global_list->move_next();
		}

		tasks_popped_since_sync = 0;
		last_sync_item = last_item;
	}

	void add_to_global_list() {
		Block* end = nullptr;
		Block* b = bottom_block;

		size_t tasks = 0;
		size_t min_k = std::numeric_limits<size_t>::max();

		while(b->get_k() > tasks) {
			if(b->get_k() - tasks < min_k) {
				min_k = b->get_k() - tasks;
			}
			tasks += b->get_num_local_items();
			end = b;
			b = b->get_prev();
			if(b == nullptr) {
				remaining_k = min_k;
				return;
			}
		}

		Block* begin = top_block.load(std::memory_order_relaxed);
		// Some blocks need to be made global (starting with oldest block)

		GlobalListItem* gli_first = nullptr;
		GlobalListItem* gli_last = nullptr;

		// First construct a list of GlobalListItems locally
		while(begin != end) {
			if(begin->has_local_items()) {
				GlobalListItem* gli = create_global_list_item();
				gli->update_block(begin);

				pc.num_global_blocks.incr();
				if(gli_last == nullptr) {
					gli_first = gli;
					gli_last = gli;
				}
				else {
					gli_last->local_link(gli);
					gli_last = gli;
				}
				begin->mark_newly_global(gli);
			}
			begin = begin->get_next();
		}

		// And then connect it to the global list
		if(gli_first != nullptr) {
			while(!global_list->link(gli_first)) {
				// Global List has been extended by other thread, process it first
				process_global_list();
			}
			global_list = gli_last;
		}

		remaining_k = min_k;
	}

	void put(Item* item) {
		tasks.store(tasks.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);

		Block* bb = bottom_block;
		pheet_assert(bb->get_next() == nullptr);
		pheet_assert(!bb->reusable());
		if(!bb->try_put(item, item->owner == this)) {
			// Check whether a merge is required
			if(bb->get_prev() != nullptr && bb->get_level() >= bb->get_prev()->get_level()) {
				bb = merge(bb);
				bottom_block = bb;
			}

			size_t l = bb->get_level();
			size_t offset = std::min((l*3) + 2, blocks.size() - 1);

			Block* new_bb = blocks[offset];
			while(!new_bb->reusable()) {
				--offset;
				new_bb = blocks[offset];
			}
			new_bb->mark_in_use();
			new_bb->set_prev(bb);
			pheet_assert(new_bb->get_next() == nullptr);
			// Make sure that if block is seen, so are the changes to the block
			bb->release_next(new_bb);

			// We know we have space in here, so no try_put needed
			new_bb->put(item, item->owner == this);

			bottom_block = new_bb;
		}
	}

	Block* merge(Block* block) {
		// Only do this if merge is actually required
		pheet_assert(block->get_prev() != nullptr && block->get_level() >= block->get_prev()->get_level());

		// Block should not be empty or try_put would have succeeded
		pheet_assert(!block->empty());

		Block* pre_merge = block->get_next();
		Block* last_merge = block->get_prev();
		while(last_merge->empty()) {
			Block* empty = last_merge;
			last_merge = last_merge->get_prev();
			if(last_merge == nullptr) {
				block->set_prev(nullptr);
				top_block.store(block, std::memory_order_release);

				pheet_assert(empty != bottom_block);
				empty->reset();
				return block;
			}
			block->set_prev(last_merge);
			last_merge->release_next(block);

			pheet_assert(empty != bottom_block);
			empty->reset();
		}
		if(last_merge->get_level() > block->get_level()) {
			return block;
		}
		pheet_assert(block == last_merge->get_next());
		Block* merged = find_free_block(block->get_level() + 1);
		merged->merge_into(last_merge, block, this, frame_regs);
		merged->mark_in_use();
		pheet_assert(merged->get_filled() <= last_merge->get_filled() + block->get_filled());
		tasks_popped_since_sync += last_merge->get_filled() + block->get_filled() - merged->get_filled();

		Block* prev = last_merge->get_prev();
		pheet_assert(prev == nullptr || prev->get_next() == last_merge);

		while(prev != nullptr && !merged->empty() && merged->get_level() >= prev->get_level()) {
			last_merge = prev;

			if(!last_merge->empty()) {
				// Only merge if the other block is not empty
				Block* merged2 = find_free_block(merged->get_level() + 1);
				merged2->merge_into(last_merge, merged, this, frame_regs);

				pheet_assert(merged2->get_filled() <= last_merge->get_filled() + merged->get_filled());
				tasks_popped_since_sync += last_merge->get_filled() + merged->get_filled() - merged2->get_filled();

				// The merged block was never made visible, we can reset it at any time
				pheet_assert(merged != bottom_block);
				merged->reset();
				merged = merged2;
				merged->mark_in_use();
			}
			prev = last_merge->get_prev();
			pheet_assert(prev == nullptr || prev->get_next() == last_merge);
		}

		// Will be released when merged block is made visible through update of top block or next pointer
		merged->set_next(block->get_next());

		// Now we need to make new blocks visible, and reset the old blocks
		if(prev == nullptr) {
			// Everything merged into one block
			// Make all changes visible now using a release
			top_block.store(merged, std::memory_order_release);
		}
		else {
			merged->set_prev(prev);

			// Make all changes visible now using a release
			prev->release_next(merged);
		}
		// Now reset old blocks
		while(last_merge != pre_merge) {
			Block* next = last_merge->get_next();
			last_merge->reset();
			last_merge = next;
		}

		return merged;
	}

	Block* find_best_block() {
		// Recount number of tasks (might change due to merging, dead tasks, so it's easiest to recalculate)
		size_t num_tasks = 0;
		size_t num_local_tasks = 0;

		Block* b = bottom_block;
		if(b->empty()) {
			Block* prev = b->get_prev();
			while(prev != nullptr && prev->empty()) {
				bottom_block = prev;
				prev->set_next(nullptr);
				b->reset();
				prev = prev->get_prev();
			}
			b = prev;
		}

		if(b == nullptr) {
			remaining_k = std::numeric_limits<size_t>::max();
			tasks_popped_since_sync = 0;

			//	bottom_block = top_block.load(std::memory_order_relaxed);
			return nullptr;
		}

		pheet_assert(!b->empty());
		Block* best = b;
		Block* next = b;
		num_tasks += b->get_filled();
		num_local_tasks += b->get_num_local_items();
		// Recalculate k
		remaining_k = b->get_k();

		b = b->get_prev();

		while(b != nullptr) {
			pheet_assert(!b->reusable());

			Block* p = b->get_prev();
			pheet_assert(p == nullptr || p->get_next() == b);
			if(b->empty()) {
				next->set_prev(p);
				if(p == nullptr) {
					// No need to order, we didn't change the block
					top_block.store(next, std::memory_order_relaxed);

					// Contains a release
					pheet_assert(b != bottom_block);
					b->reset();

					tasks = num_tasks;

					// End of list, just return best
					return best;
				}
				else {
					// No need to order, we didn't change the block
					p->set_next(next);

					// Contains a release
					pheet_assert(b != bottom_block);
					b->reset();

					b = p;
				}
			}
			else if(p != nullptr && b->get_level() >= p->get_level()) {
				pheet_assert(b != best);
				b = merge(b);
				pheet_assert(!b->reusable());
				pheet_assert(!best->reusable());
				next->set_prev(b);

				// We need to redo cycle, since merged block might be empty!
			}
			else {
				if(b->top()->strategy.prioritize(best->top()->strategy)) {
					best = b;
				}
				if(b->has_local_items()) {
					pheet_assert(b->get_k() >= num_local_tasks);
					size_t block_k = b->get_k() - num_local_tasks;
					if(block_k < remaining_k) {
						remaining_k = block_k;
					}
				}

				num_tasks += b->get_filled();
				num_local_tasks += b->get_num_local_items();
				next = b;
				b = p;
			}
		}

		tasks.store(num_tasks, std::memory_order_relaxed);

		return best;
	}

	Block* find_free_block(size_t level) {
		size_t offset = level * 3;

		if(offset >= blocks.size()) {
			do {
				size_t l = blocks.size() / 3;
				blocks.push_back(new Block(1 << l));
				pc.num_blocks_created.incr();
			}while(offset >= blocks.size());

			return blocks.back();
		}
		while(!blocks[offset]->reusable()) {
			++offset;
			if(blocks.size() == offset) {
				size_t l = offset / 3;
				Block* ret = new Block(1 << l);
				blocks.push_back(ret);
				pc.num_blocks_created.incr();
				return ret;
			}
		}
		return blocks[offset];
	}

	ParentTaskStoragePlace* parent_place;
	TaskStorage* task_storage;
	bool created_task_storage;

	ItemMemoryManager items;
	FrameMemoryManager frames;
	GlobalListItemMemoryManager global_list_items;

	// Stores 3 blocks per level.
	// 2 would be enough in general, but with 3 less synchronization is required
	std::vector<Block*> blocks;
	std::atomic<Block*> top_block;
	Block* bottom_block;

	Frame* current_frame;
	std::unordered_map<Frame*, FrameReg> frame_regs;

//	Block* best_block;

	size_t remaining_k;
	size_t tasks_popped_since_sync;
	std::atomic<size_t> tasks;

	GlobalListItem* global_list;

	size_t last_item;
	size_t last_sync_item;
};

} /* namespace pheet */
#endif /* KDELAYEDLSMLOCALITYTASKSTORAGEPLACE_H_ */
