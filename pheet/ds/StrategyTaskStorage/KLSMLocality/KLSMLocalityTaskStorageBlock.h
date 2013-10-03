/*
 * KLSMLocalityTaskStorageBlock.h
 *
 *  Created on: Sep 24, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGEBLOCK_H_
#define KLSMLOCALITYTASKSTORAGEBLOCK_H_

#include <atomic>
#include <algorithm>

namespace pheet {

/*
 * Block for task storage. Stores items in sorted order (by strategy)
 * Blocks are stored as a linked list
 * A block can be merged with its successor
 * Automatically cleans up dead and taken Tasks
 *
 * All write operations are only safe for the owner
 * Items can be read out by any thread
 *
 * Owner iterates through blocks backwards using back pointers (not safe for other threads)
 * Other threads iterate through blocks forward using next pointers
 * Blocks are sorted from largest to smallest. The local thread needs to add items to the smallest
 * block, and merge with larger, whereas it makes more sense for other threads to start with the largest block
 */
template <class Pheet, class Item>
class KLSMLocalityTaskStorageBlock {
public:
	typedef KLSMLocalityTaskStorageBlock<Pheet, Item> Self;

	KLSMLocalityTaskStorageBlock(size_t size)
	:size(size), level(0), level_boundary(1), next(nullptr), k(std::numeric_limits<size_t>::max()) {
		data = new std::atomic<Item*>[size];
	}
	~KLSMLocalityTaskStorageBlock() {
		delete[] data;
	}

	/*
	 * Assumes given item is already registered in frame if necessary
	 */
	bool try_put(Item* item) {
		size_t f = filled.load(std::memory_order_relaxed);
		if(f == size) {
			return false;
		}
		else if(f == 0 ||
				item->strategy.prioritize(data[f-1].load(std::memory_order_relaxed)->strategy)) {
			data[f].store(item, std::memory_order_relaxed);
			// If new value for filled is seen, so is the item stored in it
			filled.store(f + 1, std::memory_order_release);

			if(f + 1 > (level_boundary)) {
				++level;
				level_boundary <<= 1;
			}
			return true;
		}
		return false;
	}

	/*
	 * Assumes given item is already registered in frame if necessary
	 */
	void put(Item* item) {
		size_t f = filled.load(std::memory_order_relaxed);
		pheet_assert(f < size);
		pheet_assert(f == 0 ||
				item->strategy.prioritize(data[f-1].load(std::memory_order_relaxed)->strategy));
		data[f].store(item, std::memory_order_relaxed);
		// If new value for filled is seen, so is the item stored in it
		filled.store(f + 1, std::memory_order_release);

		if(f + 1 > (level_boundary)) {
			++level;
			level_boundary <<= 1;
		}
	}

	Item* top() {
		size_t f = filled.load(std::memory_order_relaxed);
		pheet_assert(f > 0);
		return data[f - 1].load(std::memory_order_relaxed);
	}

	Item* get_item(size_t pos) {
		pheet_assert(pos < size);
		return data[pos].load(std::memory_order_relaxed);
	}

	void pop() {
		size_t f = filled.load(std::memory_order_relaxed);
		pheet_assert(f > 0);
		filled.store(f-1, std::memory_order_relaxed);
		if(f-1 < (level_boundary >> 1)) {
			pheet_assert(level > 0);
			--level;
			pheet_assert(level_boundary > 0);
			level_boundary >>= 1;
		}
	}

	/*
	 * Scans the top tasks until either the block is empty or the top is not dead and not taken
	 */
	template <class Place, class Hashtable>
	void pop_taken_and_dead(Place* local_place, Hashtable& frame_regs) {
		size_t f = filled.load(std::memory_order_relaxed);
		while(f > 0) {
			size_t f2 = f - 1;
			Item* item = data[f2].load(std::memory_order_relaxed);
			if(item->last_phase.load(std::memory_order_relaxed) == -1) {
				if(item->strategy.dead_task()) {
					// If we don't succeed someone else will, either way we won't execute the task
					if(item->owner == local_place) {
						item->take_and_delete(local_place);
					}
					else {
						item->take_and_delete(frame_regs[item->frame.load(std::memory_order_relaxed)]);
					}
				}
				else {
					break;
				}
			}
			f = f2;
		}
		filled.store(f, std::memory_order_relaxed);

		size_t next_boundary = level_boundary >> 1;
		while(f <= next_boundary) {
			pheet_assert(level > 0);
			--level;
			pheet_assert(next_boundary > 0);
			next_boundary >>= 1;
		}
		level_boundary = next_boundary;
	}

	bool empty() {
		size_t f = filled.load(std::memory_order_acquire);
		return f == 0;
	}

	size_t get_filled() {
		return filled.load(std::memory_order_relaxed);
	}

	size_t acquire_filled() {
		return filled.load(std::memory_order_acquire);
	}

	void drop_empty() {
		Self* n = next.load(std::memory_order_relaxed);
		while(n != nullptr && n->filled == 0) {
			n = n->next.load(std::memory_order_relaxed);
		}
		next.store(n, std::memory_order_relaxed);
	}

	/*
	 * Not thread-safe. Only to be called by owner
	 */
	Self* get_prev() {
		return prev;
	}

	void set_prev(Self* prev) {
		this->prev = prev;
	}

	Self* get_next() {
		return next.load(std::memory_order_relaxed);
	}

	Self* acquire_next() {
		return next.load(std::memory_order_acquire);
	}

	void set_next(Self* next) {
		this->next.store(next, std::memory_order_relaxed);
	}

	void release_next(Self* next) {
		this->next.store(next, std::memory_order_release);
	}

	/*
	 * Merges the two given blocks into this block
	 * Assumes this block is empty
	 */
	template <class Place, class Hashtable>
	void merge_into(Self* left, Self* right, Place* local_place, Hashtable& frame_regs) {
		pheet_assert(filled.load(std::memory_order_relaxed) == 0);
		pheet_assert(left->level <= right->level);

		size_t f = 0;
		size_t l_max = left->filled.load(std::memory_order_relaxed);
		size_t r_max = right->filled.load(std::memory_order_relaxed);
		size_t l = left->find_next_non_dead(0, local_place, frame_regs);
		size_t r = right->find_next_non_dead(0, local_place, frame_regs);

		while(l != l_max && r != r_max) {
			Item* l_item = left->data[l].load(std::memory_order_relaxed);
			Item* r_item = right->data[l].load(std::memory_order_relaxed);
			if(l_item->strategy.prioritize(
					r_item->strategy)) {
				data[f].store(r_item, std::memory_order_relaxed);
				r = right->find_next_non_dead(r + 1, local_place, frame_regs);
			}
			else {
				data[f].store(l_item, std::memory_order_relaxed);
				l = left->find_next_non_dead(l + 1, local_place, frame_regs);
			}
			++f;
		}

		if(l != l_max) {
			pheet_assert(r == r_max);
			do {
				Item* l_item = left->data[l].load(std::memory_order_relaxed);
				data[f].store(l_item, std::memory_order_relaxed);
				l = left->find_next_non_dead(l + 1, local_place, frame_regs);
				++f;
			} while(l != l_max);
		}
		else {
			while(r != r_max) {
				Item* r_item = right->data[l].load(std::memory_order_relaxed);
				data[f].store(r_item, std::memory_order_relaxed);
				r = right->find_next_non_dead(r + 1, local_place, frame_regs);
				++f;
			}
		}

		// No synchronization needed. Merged list is made visible using a release
		filled.store(f, std::memory_order_relaxed);

		pheet_assert(level == 0);
		pheet_assert(level_boundary == 1);
		while(f > (level_boundary)) {
			++level;
			level_boundary <<= 1;
		}
	}

	size_t get_level() {
		return level;
	}

	void reset() {
		// Make sure new list is visible when this list is encountered as empty
		filled.store(0, std::memory_order_release);

		level = 0;
		level_boundary = 1;
		// Make sure new list is visible when successors are not available any more
		next.store(nullptr, std::memory_order_release);
		prev = nullptr;

		k = std::numeric_limits<size_t>::max();
	}

private:
	template <class Place, class Hashtable>
	size_t find_next_non_dead(size_t offset, Place* local_place, Hashtable& frame_regs) {
		size_t f = filled.load(std::memory_order_relaxed);
		while(offset < f) {
			Item* item = data[offset].load(std::memory_order_relaxed);
			if(item->last_phase == -1) {
				if(item->strategy.dead_task()) {
					// If we don't succeed someone else will, either way we won't execute the task
					if(item->owner == local_place) {
						item->take_and_delete(local_place);
					}
					else {
						item->take_and_delete(frame_regs[item->frame.load(std::memory_order_relaxed)]);
					}
				}
				else {
					break;
				}
			}
			++offset;
		}
		return offset;
	}

	std::atomic<Item*>* data;
	std::atomic<size_t> filled;
	size_t size;
	size_t level;
	size_t level_boundary;

	std::atomic<Self*> next;
	Self* prev;

	size_t k;
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEBLOCK_H_ */
