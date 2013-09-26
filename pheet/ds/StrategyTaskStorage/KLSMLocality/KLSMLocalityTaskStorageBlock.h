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
 */
template <class Pheet, typename Item>
class KLSMLocalityTaskStorageBlock {
public:
	typedef KLSMLocalityTaskStorageBlock<Pheet, Item> Self;

	KLSMLocalityTaskStorageBlock(size_t size)
	:size(size), level(0), level_boundary(1), next(nullptr), k(std::numeric_limits<size_t>::max()) {
		data = new std::atomic<Item*>[size];
		// Initialized to nullptr in case thread sees an update to filled before the pointer is written
		for(size_t i = 0; i < size; ++i) {
			data[i].store(nullptr, std::memory_order_relaxed);
		}
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
				item->strategy.prioritize(data[f-1]->strategy)) {
			// No ordering constraints whatsoever. Put is only called on local blocks
			// And we do not give any guarantees on items seen in local blocks
			// Items may only be reused if no thread is registered to a frame for the given phase
			// and registering for a new phase triggers a synchronization
			data[f].store(item, std::memory_order_relaxed);
			filled.store(f + 1, std::memory_order_relaxed);

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
	bool put(Item* item) {
		size_t f = filled.load(std::memory_order_relaxed);
		pheet_assert(f < size);
		pheet_assert(f == 0 ||
				item->strategy.prioritize(data[f-1]->strategy));
		// No ordering constraints whatsoever. Put is only called on local blocks
		// And we do not give any guarantees on items seen in local blocks
		// Items may only be reused if no thread is registered to a frame for the given phase
		// and registering for a new phase triggers a synchronization
		data[f].store(item, std::memory_order_relaxed);
		filled.store(f + 1, std::memory_order_relaxed);

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
	void pop_taken_and_dead() {
		size_t f = filled.load(std::memory_order_relaxed);
		while(f > 0) {
			size_t f2 = f - 1;
			Item* item = data[f2].load(std::memory_order_relaxed);
			if(item->last_phase.load(std::memory_order_relaxed) == -1) {
				if(item->strategy.dead_task()) {
					// If we don't succeed someone else will, either way we won't execute the task
					item->take_and_delete();
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
		size_t f = filled.load(std::memory_order_relaxed);
		return f == 0;
	}

	size_t get_filled() {
		return filled.load(std::memory_order_relaxed);
	}


	/*
	 * Informs whether this block needs to be merged with the next block
	 * (Also cleans out empty blocks)
	 */
	bool needs_merge() {
		Self* n = next.load(std::memory_order_relaxed);
		while(n != nullptr && n->level <= level) {
			if(n->filled != 0) {
				return true;
			}
			n = n->next;
			next.store(n, std::memory_order_relaxed);
		}
		return false;
	}

	void drop_empty() {
		Self* n = next.load(std::memory_order_relaxed);
		while(n != nullptr && n->filled == 0) {
			n = n->next;
		}
		next.store(n, std::memory_order_relaxed);
	}

	Self* get_next() {
		return next.load(std::memory_order_relaxed);
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
	void merge_into(Self* left, Self* right) {
		pheet_assert(filled.load(std::memory_order_relaxed) == 0);
		pheet_assert(right->next.load(std::memory_order_relaxed) == left);
		pheet_assert(right->needs_merge());

		size_t f = 0;
		size_t l_max = left->filled.load(std::memory_order_relaxed);
		size_t r_max = right->filled.load(std::memory_order_relaxed);
		size_t l = left->find_next_non_dead(0);
		size_t r = right->find_next_non_dead(0);

		while(l != l_max && r != r_max) {
			Item* l_item = left->data[l].load(std::memory_order_relaxed);
			Item* r_item = right->data[l].load(std::memory_order_relaxed);
			if(l_item->strategy->prioritize(
					r_item->strategy)) {
				data[f].store(r_item, std::memory_order_relaxed);
				r = right->find_next_non_dead(r + 1);
			}
			else {
				data[f].store(l_item, std::memory_order_relaxed);
				l = left->find_next_non_dead(l + 1);
			}
			++f;
		}

		if(l != l_max) {
			pheet_assert(r == r_max);
			do {
				Item* l_item = left->data[l].load(std::memory_order_relaxed);
				data[f].store(l_item, std::memory_order_relaxed);
				l = left->find_next_non_dead(l + 1);
			} while(l != l_max);
		}
		else {
			while(r != r_max) {
				Item* r_item = right->data[l].load(std::memory_order_relaxed);
				data[f].store(r_item, std::memory_order_relaxed);
				r = right->find_next_non_dead(r + 1);
			}
		}

		// No synchronization needed unless we are in global list
		filled.store(f + 1, std::memory_order_relaxed);

		pheet_assert(level == 0);
		pheet_assert(level_boundary == 1);
		if(f > (level_boundary)) {
			++level;
			level_boundary <<= 1;
		}
	}

	size_t get_level() {
		return level;
	}

	bool empty() {
		return filled.load(std::memory_order_relaxed) == 0;
	}

	void reset() {
		filled.store(0, std::memory_order_relaxed);

		level = 0;
		level_boundary = 1;
		// Ordering not required. We don't give any guarantees when iterating through local blocks
		next.store(nullptr, std::memory_order_relaxed);

		k = std::numeric_limits<size_t>::max();
	}

private:
	size_t find_next_non_dead(size_t offset) {
		size_t f = filled.load(std::memory_order_relaxed);
		while(offset < f) {
			Item* item = data[offset].load(std::memory_order_relaxed);
			if(item->last_phase == 0) {
				if(item->strategy.dead_task()) {
					// If we don't succeed someone else will, either way we won't execute the task
					item->take_and_delete();
				}
				else {
					break;
				}
			}
			++offset;
		}
		return offset;
	}

	std::atomic<Item*> data;
	std::atomic<size_t> filled;
	size_t size;
	size_t level;
	size_t level_boundary;

	std::atomic<Self*> next;

	size_t k;
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEBLOCK_H_ */
