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
#include "KLSMLocalityTaskStorageGlobalListItem.h"

#include <pheet/memory/BlockItemReuse/BlockItemReuseMemoryManager.h>
#include <pheet/memory/ItemReuse/ItemReuseMemoryManager.h>

#include <limits>
#include <unordered_map>

namespace pheet {

template <class Pheet, class TaskStorage, class ParentTaskStoragePlace, class Strategy>
class KLSMLocalityTaskStoragePlace : public ParentTaskStoragePlace::BaseTaskStoragePlace {
public:
	typedef KLSMLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy> Self;

	typedef typename ParentTaskStoragePlace::BaseTaskStoragePlace BaseTaskStoragePlace;
	typedef KLSMLocalityTaskStorageFrame<Pheet> Frame;
	typedef KLSMLocalityTaskStorageFrameRegistration<Pheet, Frame> FrameReg;
	typedef typename ParentTaskStoragePlace::BaseItem BaseItem;
	typedef KLSMLocalityTaskStorageItem<Pheet, Self, Frame, FrameReg, BaseItem, Strategy> Item;
	typedef KLSMLocalityTaskStorageBlock<Pheet, Item> Block;
	typedef KLSMLocalityTaskStorageGlobalListItem<Pheet, Block> GlobalListItem;

	typedef typename BaseItem::T T;

	typedef BlockItemReuseMemoryManager<Pheet, Item, KLSMLocalityTaskStorageItemReuseCheck<Item, Frame> > ItemMemoryManager;
	typedef ItemReuseMemoryManager<Pheet, Frame, KLSMLocalityTaskStorageFrameReuseCheck<Frame> > FrameMemoryManager;
	typedef ItemReuseMemoryManager<Pheet, GlobalListItem, KLSMLocalityTaskStorageGlobalListItemReuseCheck<GlobalListItem> > GlobalListItemMemoryManager;

	typedef typename ParentTaskStoragePlace::PerformanceCounters PerformanceCounters;

	KLSMLocalityTaskStoragePlace(ParentTaskStoragePlace* parent_place)
	:pc(parent_place->pc),
	 parent_place(parent_place), top_block_shared(nullptr), bottom_block_shared(nullptr),
	 best_block(nullptr), best_block_known(true),
	 needs_rescan(false),
	 current_frame(&(frames.acquire_item())),
	 remaining_k(std::numeric_limits<size_t>::max()), enforce_global(false), tasks(0) {

		// Get central task storage at end of initialization (not before,
		// since this place becomes visible as soon as we retrieve it)
		task_storage = TaskStorage::get(this, parent_place->get_central_task_storage(), created_task_storage);

		global_list = task_storage->get_global_list_head();

		// Fill in blocks usable for storing a single item
		bottom_block = find_free_block(0);
		top_block.store(bottom_block, std::memory_order_relaxed);
		bottom_block->mark_in_use();
	}

	~KLSMLocalityTaskStoragePlace() {
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
			process_global_list();

			pheet_assert(best_block_known);
			// We can assume that boundary is stored in this task storage, so we just get the best item
			Block* best = best_block;

			if(best == nullptr || item->is_taken()) {
				return nullable_traits<T>::null_value;
			}

			Item* best_item = best->top();

			// Take does not do any deregistration, this is done by pop_taken_and_dead
			T data = best_item->take();

			// Check level of best block
			size_t level = best->get_level();

			// There is at least one taken task now, let's do a cleanup
			best->pop_taken_and_dead(this, frame_regs);
			best_block_known = false;
			best_block = nullptr;

			if(best->empty() || best->get_level() != level) {
				scan();
			}
			// Check whether we can merge blocks first
			else if(needs_rescan) {
				scan();
			}

			if(!best_block_known) {
				find_best_block();
			}

			// Will be automatically called in scan
	//		find_best_block();

			// Check whether we succeeded
			if(data != nullable_traits<T>::null_value) {
				tasks.store(tasks.load(std::memory_order_relaxed) - 1, std::memory_order_relaxed);
				return data;
			}
		}

		pheet_assert(best_block_known);
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
											pc.num_spied_tasks.incr();

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
								pc.num_spied_tasks.incr();
								pc.num_spied_global_tasks.incr();

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
	}

	void add_to_global_list() {
		enforce_global = false;
		Block* begin = top_block.load(std::memory_order_relaxed);
		Block* b = begin;
		// Some blocks need to be made global (starting with oldest block)

		GlobalListItem* gli_first = nullptr;
		GlobalListItem* gli_last = nullptr;

		// First construct a list of GlobalListItems locally
		while(b != nullptr) {
			GlobalListItem* gli = create_global_list_item();
			gli->update_block(b);

			pc.num_global_blocks.incr();
			if(gli_last == nullptr) {
				gli_first = gli;
				gli_last = gli;
			}
			else {
				gli_last->local_link(gli);
				gli_last = gli;
			}
			b->mark_newly_global(gli);
			b = b->get_next();
		}

		// And then connect it to the global list
		if(gli_first != nullptr) {
			while(!global_list->link(gli_first)) {
				// Global List has been extended by other thread, process it first
				process_global_list();
			}
			global_list = gli_last;
		}

		// Now add each block to shared list
		b = begin;
		while(b != nullptr) {
			Block* next = b->get_next();
			// To ensure all tasks remain visible to other threads
			top_block.store(next, std::memory_order_relaxed);

			link_to_shared_list(b);
			b = next;
		}

		// Now find new block for local list and reset local list
		Block* new_bb = find_free_block(blocks.size() >> 2);
		new_bb->mark_in_use();
		pheet_assert(new_bb->get_prev() == nullptr);
		pheet_assert(new_bb->get_next() == nullptr);

		bottom_block = new_bb;
		top_block.store(bottom_block, std::memory_order_release);

		// Restart counting
		remaining_k = std::numeric_limits<size_t>::max();

		if(!best_block_known) {
			find_best_block();
		}
	}

	void link_to_shared_list(Block* block) {
		Block* next = nullptr;
		Block* b = bottom_block_shared;

		while(b != nullptr && block->get_max_level() > b->get_max_level()) {
			next = b;
			b = b->get_prev();
		}

		if(next == nullptr) {
			bottom_block_shared = block;
		}
		else {
			next->set_prev(block);
		}
		block->set_prev(b);
		block->release_next(next);
		if(b == nullptr) {
			top_block_shared = block;
		}
		else {
			b->set_next(block);

			if(b->get_level() <= block->get_level() || b->get_max_level() == block->get_max_level()) {
				merge_shared(block);
			}
		}
		pheet_assert(bottom_block_shared->get_next() == nullptr);
		pheet_assert(top_block_shared->get_prev() == nullptr);
	}

	void put(Item* item) {
		pheet_assert(best_block_known);

		tasks.store(tasks.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);

		Block* bb = bottom_block;
		pheet_assert(bb->get_next() == nullptr);
		pheet_assert(!bb->reusable());
		if(!bb->try_put(item, item->owner == this)) {
			// Lazy merging, blocks will only be merged when looking for the best block
			// or when running out of blocks to use

			size_t l = bb->get_level();
			size_t offset = l << 2;

			Block* new_bb = nullptr;
			size_t found = 0;
			for(int i = 0; i < 4; ++i) {
				if(blocks[offset + i]->reusable()) {
					++found;
					new_bb = blocks[offset + i];
				}
			}
			if(found == 1) {
				// Can't take the last block or we might fail when trying to merge blocks
				// We need to scan existing blocks to free more blocks
				scan();
				bb = bottom_block;

				for(int i = 0; i < 4; ++i) {
					if(blocks[offset + i]->reusable()) {
						// Now we can just assume there are at least 2 free blocks
						new_bb = blocks[offset + i];
						break;
					}
				}
			}
			pheet_assert(found != 0);

			new_bb->mark_in_use();
			new_bb->set_prev(bb);
			pheet_assert(new_bb->get_next() == nullptr);
			// Make sure that if block is seen, so are the changes to the block
			pheet_assert(bb != bottom_block_shared);
			bb->release_next(new_bb);
			bottom_block = new_bb;

			// We know we have space in here, so no try_put needed
			bottom_block->put(item, item->owner == this);

			if(best_block == nullptr || (best_block != bottom_block && item->strategy.prioritize(best_block->top()->strategy))) {
				best_block = bottom_block;
			}
		}
		else if(best_block == nullptr || (best_block != bb && item->strategy.prioritize(best_block->top()->strategy))) {
			best_block = bb;
		}
	}

	Block* merge(Block* block) {
		// Only do this if merge is actually required
		pheet_assert(block->get_prev() != nullptr && block->get_level() >= block->get_prev()->get_level());

		// Block should not be empty or try_put would have succeeded
		pheet_assert(!block->empty());

		Block* pre_merge = block->get_next();
		Block* last_merge = block->get_prev();
		pheet_assert(!last_merge->empty());
		pheet_assert(block == last_merge->get_next());
		Block* merged = find_free_block(std::max(block->get_level() + 1, block->get_max_level()));
		merged->merge_into(last_merge, block, this, frame_regs);
		merged->mark_in_use();
		pheet_assert(merged->get_filled() <= last_merge->get_filled() + block->get_filled());
		pheet_assert(merged->get_owned_filled() <= last_merge->get_owned_filled() + block->get_owned_filled());
		pheet_assert(merged->get_k() >= std::min(last_merge->get_k() - block->get_owned_filled(), block->get_k()));

		Block* prev = last_merge->get_prev();
		pheet_assert(prev == nullptr || prev->get_next() == last_merge);

		while(prev != nullptr && !merged->empty() &&
				(merged->get_level() >= prev->get_level())) {
			last_merge = prev;

			pheet_assert(!last_merge->empty());

			// Only merge if the other block is not empty
			// Each subsequent merge takes a block one greater than the previous one to ensure we don't run out of blocks
			Block* merged2 = find_free_block(merged->get_max_level() + 1);
			merged2->merge_into(last_merge, merged, this, frame_regs);

			pheet_assert(merged2->get_filled() <= last_merge->get_filled() + merged->get_filled());
			pheet_assert(merged2->get_owned_filled() <= last_merge->get_owned_filled() + merged->get_owned_filled());
			pheet_assert(merged2->get_k() >= std::min(last_merge->get_k() - merged->get_owned_filled(), merged->get_k()));

			// The merged block was never made visible, we can reset it at any time
			pheet_assert(merged != bottom_block);
			pheet_assert(merged != bottom_block_shared);
			merged->reset();
			merged = merged2;
			merged->mark_in_use();

			prev = last_merge->get_prev();
			pheet_assert(prev == nullptr || prev->get_next() == last_merge);
		}

		// Will be released when merged block is made visible through update of top block or next pointer
		merged->set_next(block->get_next());

		// Now we need to make new blocks visible, and reset the old blocks
		if(prev != nullptr) {
			merged->set_prev(prev);

			// Make all changes visible now using a release
			pheet_assert(prev != bottom_block_shared);
			prev->release_next(merged);
		}
		else {
			// Everything merged into one block
			// Make all changes visible now using a release
			top_block.store(merged, std::memory_order_release);
		}

		if(pre_merge == nullptr) {
			bottom_block = merged;
		}
		else {
			pheet_assert(pre_merge != top_block.load(std::memory_order_relaxed));
			pheet_assert(pre_merge != top_block_shared);
			pre_merge->set_prev(merged);
		}

		// Now reset old blocks
		while(last_merge != pre_merge) {
			Block* next = last_merge->get_next();
			if(last_merge == best_block) {
				best_block = nullptr;
				best_block_known = false;
			}
			pheet_assert(last_merge != bottom_block);
			last_merge->reset();
			last_merge = next;
		}

		if(merged->get_owned_filled() > merged->get_k()) {
			enforce_global = true;
		}

		return merged;
	}

	Block* merge_shared(Block* block) {
		// Only do this if merge is actually required
		pheet_assert(block->get_prev() != nullptr &&
				(block->get_level() >= block->get_prev()->get_level() ||
						block->get_max_level() == block->get_prev()->get_max_level()));

		pheet_assert(bottom_block_shared->get_next() == nullptr);
		pheet_assert(top_block_shared->get_prev() == nullptr);

		// Block should not be empty or try_put would have succeeded
		pheet_assert(!block->empty());

		Block* pre_merge = block->get_next();
		Block* last_merge = block->get_prev();
		pheet_assert(!last_merge->empty());
		pheet_assert(block == last_merge->get_next());
		// Choose a block big enough to fit data of both blocks, but never smaller than the smaller of
		// the two blocks (No point in using a smaller block, larger is definitely free)
		Block* merged = find_free_block(std::max(block->get_level() + 1, std::max(last_merge->get_level() + 1, block->get_max_level())));
		merged->merge_into(last_merge, block, this, frame_regs);
		merged->mark_in_use();
		pheet_assert(merged->get_filled() <= last_merge->get_filled() + block->get_filled());
		pheet_assert(merged->get_owned_filled() <= last_merge->get_owned_filled() + block->get_owned_filled());
		pheet_assert(merged->get_k() >= std::min(last_merge->get_k() - block->get_owned_filled(), block->get_k()));

		Block* prev = last_merge->get_prev();
		pheet_assert(prev == nullptr || prev->get_next() == last_merge);

		GlobalListItem* gli = block->get_global_list_item();
		if(gli == nullptr) {
			gli = last_merge->get_global_list_item();
		}

		while(prev != nullptr && !merged->empty() &&
				(merged->get_level() >= prev->get_level() || merged->get_max_level() == prev->get_max_level())) {
			last_merge = prev;

			pheet_assert(!last_merge->empty());

			// Only merge if the other block is not empty
			// Each subsequent merge takes a block one greater than the previous one to ensure we don't run out of blocks
			Block* merged2 = find_free_block(merged->get_max_level() + 1);
			merged2->merge_into(last_merge, merged, this, frame_regs);

			if(gli == nullptr) {
				gli = last_merge->get_global_list_item();
			}
			pheet_assert(merged2->get_filled() <= last_merge->get_filled() + merged->get_filled());
			pheet_assert(merged2->get_owned_filled() <= last_merge->get_owned_filled() + merged->get_owned_filled());
			pheet_assert(merged2->get_k() >= std::min(last_merge->get_k() - merged->get_owned_filled(), merged->get_k()));

			// The merged block was never made visible, we can reset it at any time
			pheet_assert(merged != bottom_block);
			pheet_assert(merged != bottom_block_shared);
			merged->reset();
			merged = merged2;
			merged->mark_in_use();

			prev = last_merge->get_prev();
			pheet_assert(prev == nullptr || prev->get_next() == last_merge);
		}

		// Will be released when merged block is made visible through update of top block or next pointer
		merged->set_next(block->get_next());

		// Now we need to make new blocks visible, and reset the old blocks
		if(prev != nullptr) {
			merged->set_prev(prev);

			// Make all changes visible now using a release
			prev->release_next(merged);
		}
		else {
			// Everything merged into one block
			// Make all changes visible now using a release
			top_block_shared = merged;
		}

		if(pre_merge == nullptr) {
			bottom_block_shared = merged;
		}
		else {
			pre_merge->set_prev(merged);
		}

		// Now we need to update the global list item if necessary
		if(gli != nullptr) {
			gli->update_block(merged);
		}

		// Now reset old blocks
		while(last_merge != pre_merge) {
			Block* next = last_merge->get_next();
			if(last_merge == best_block) {
				best_block = nullptr;
				best_block_known = false;
			}
			pheet_assert(last_merge != bottom_block_shared);
			last_merge->reset();
			last_merge = next;
		}
		pheet_assert(bottom_block_shared->get_next() == nullptr);
		pheet_assert(top_block_shared->get_prev() == nullptr);

		return merged;
	}

	void find_best_block() {
		Block* b = bottom_block;
		pheet_assert(b != nullptr);

		do {
			if(!b->empty() &&
					(best_block == nullptr ||
						b->top()->strategy.prioritize(best_block->top()->strategy))) {
				best_block = b;
			}
			b = b->get_prev();
		} while(b != nullptr);

		b = top_block_shared;
		while(b != nullptr) {
			if(!b->empty() &&
					(best_block == nullptr ||
						b->top()->strategy.prioritize(best_block->top()->strategy))) {
				best_block = b;
			}
			b = b->get_next();
		}

		best_block_known = true;
	}

	/*
	 * Goes through local lists of blocks and merges all mergeable blocks and removes empty
	 * blocks
	 */
	void scan() {
		// Recount number of tasks (might change due to merging, dead tasks, so it's easiest to recalculate)
		size_t num_tasks = 0;
		remaining_k = std::numeric_limits<size_t>::max();

		Block* prev = nullptr;
		Block* b = top_block.load(std::memory_order_relaxed);
		pheet_assert(b != nullptr);

		do {
			num_tasks += b->get_filled();

			Block* next = b->get_next();
			if(b->empty()) {
				if(next != nullptr) {
					next->set_prev(prev);
					if(prev != nullptr) {
						// No need to order, we didn't change the block
						prev->set_next(next);
					}
					else {
						// No need to order, we didn't change the block
						top_block.store(next, std::memory_order_relaxed);
					}
					b->reset();
					b = next;
				}
				else {
					// Never remove first (bottom) block if it is empty, it is used for adding items
					pheet_assert(next == nullptr || next->get_prev() == b);
					prev = b;
					b = next;
				}
			}
			else if(prev != nullptr && b->get_level() >= prev->get_level()) {
				b = merge(b);

				// Repeat cycle since block might now be empty
				pheet_assert(b->get_next() == next);
				prev = b->get_prev();
			}
			else {
				remaining_k = std::min(remaining_k - b->get_owned_filled(), b->get_k());

				pheet_assert(next == nullptr || next->get_prev() == b);
				prev = b;
				b = next;
			}
		} while(b != nullptr);

		prev = nullptr;
		b = top_block_shared;

		// Now go through list of shared blocks
		// No need to order memory accesses list is not accessed globally through these pointers
		while(b != nullptr) {
			num_tasks += b->get_filled();

			Block* next = b->get_next();
			if(b->empty()) {
				if(next != nullptr) {
					next->set_prev(prev);
					if(prev != nullptr) {
						prev->set_next(next);
					}
					else {
						top_block_shared = next;
					}
				}
				else if(prev != nullptr) {
					prev->set_next(nullptr);
					pheet_assert(bottom_block_shared == b);
					bottom_block_shared = prev;
				}
				else {
					pheet_assert(top_block_shared == b);
					pheet_assert(bottom_block_shared == b);
					top_block_shared = nullptr;
					bottom_block_shared = nullptr;
				}
				b->reset();
				b = next;
			}
			else if(prev != nullptr && b->get_level() >= prev->get_level()) {
				b = merge_shared(b);

				// Repeat cycle since block might now be empty
				pheet_assert(b->get_next() == next);
				prev = b->get_prev();
			}
			else {
				pheet_assert(next == nullptr || next->get_prev() == b);
				prev = b;
				b = next;
			}
		}

		tasks.store(num_tasks, std::memory_order_relaxed);
		needs_rescan = false;

		if(!best_block_known) {
			find_best_block();
		}

		if(enforce_global) {
			add_to_global_list();
		}
	}

	Block* find_free_block(size_t level) {
		size_t offset = level << 2;

		if(offset >= blocks.size()) {
			do {
				size_t l = blocks.size() >> 2;
				size_t s = (1 << l);
				for(int i = 0; i < 4; ++i) {
					blocks.push_back(new Block(s, l));
					pc.num_blocks_created.incr();
				}
			}while(offset >= blocks.size());

			return blocks.back();
		}
		for(int i = 0; i < 4; ++i) {
			if(blocks[offset + i]->reusable()) {
				return blocks[offset + i];
			}
		}
		return nullptr;
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
	Block* top_block_shared;
	Block* bottom_block_shared;
	Block* best_block;
	bool best_block_known;
	bool needs_rescan;

	Frame* current_frame;
	std::unordered_map<Frame*, FrameReg> frame_regs;

//	Block* best_block;

	size_t remaining_k;
	bool enforce_global;
	std::atomic<size_t> tasks;

	GlobalListItem* global_list;
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEPLACE_H_ */
