/*
 * Strategy2BaseTaskStorageDataBlock.h
 *
 *  Created on: Aug 13, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef STRATEGY2BASETASKSTORAGEDATABLOCK_H_
#define STRATEGY2BASETASKSTORAGEDATABLOCK_H_

#include "Strategy2BaseTaskStorageItem.h"

#include <atomic>

namespace pheet {

/**
 * Doubly linked list of blocks of items
 * Iterating through next pointers is thread-safe in the sense that always a valid block will be returned
 * Be aware that blocks might be reused while iterating! Since spying does not give any guarantees that
 * all tasks will be seen, this is allowed.
 * Prev pointers may only be used by local thread. Prev pointers are never reset, so the predecessor is
 * still pointed to even if it is already reused.
 */
template <class Pheet, class Place, class TaskStorage, typename TT, size_t BlockSize>
class Strategy2BaseTaskStorageDataBlock {
public:
	typedef Strategy2BaseTaskStorageDataBlock<Pheet, Place, TaskStorage, TT, BlockSize> Self;
	typedef Strategy2BaseTaskStorageBaseItem<Pheet, TaskStorage, TT> BaseItem;

	Strategy2BaseTaskStorageDataBlock()
	:reuse(true) {}
	~Strategy2BaseTaskStorageDataBlock() {}

	void link(Self* prev) {
		reuse.store(false, std::memory_order_relaxed);
		this->prev = prev;
		// When new value of next pointer becomes visible, so do all the updates to the linked list
		// The new linked list will therefore be found
		this->next.store(nullptr, std::memory_order_release);

		if(prev != nullptr) {
			pheet_assert(prev->next.load(std::memory_order_relaxed) == nullptr);
			offset.store(prev->offset + BlockSize, std::memory_order_relaxed);

			// When next pointer of predecessor is read (as not null), all initialization happened before
			prev->next.store(this, std::memory_order_release);
		}
		else {
			offset.store(0, std::memory_order_relaxed);
		}
	}

	BaseItem* get(size_t pos) {
		pheet_assert(pos - offset.load(std::memory_order_relaxed) < BlockSize);
		return items[pos - offset.load(std::memory_order_relaxed)].load(std::memory_order_relaxed);
	}

	/**
	 * Gets item without modifying index by offset
	 */
	BaseItem* direct_get(size_t pos) {
		return items[pos].load(std::memory_order_relaxed);
	}

	size_t get_block_offset() {
		return offset.load(std::memory_order_relaxed);
	}

	void put(BaseItem* item, size_t pos) {
		pheet_assert(pos - offset.load(std::memory_order_relaxed) < BlockSize);
		items[pos - offset.load(std::memory_order_relaxed)].store(item, std::memory_order_relaxed);
	}

	bool fits(size_t pos) {
		return pos - offset.load(std::memory_order_relaxed) < BlockSize;
	}

	int compare(size_t pos) {
		ptrdiff_t diff = (ptrdiff_t)(pos - offset.load(std::memory_order_relaxed));
		if(diff < 0) {
			return -1;
		}
		else if(diff >= BlockSize) {
			return 1;
		}
		return 0;
	}

	Self* get_next() {
		return next.load(std::memory_order_relaxed);
	}

	Self* get_prev() {
		return prev;
	}

	void mark_reusable() {
		pheet_assert(!reuse.load(std::memory_order_relaxed));
		// To make sure the update to the pointers becomes visible before
		// block is reused do a release
		reuse.store(true, std::memory_order_release);
	}

	bool is_reusable() const {
		return reuse.load(std::memory_order_acquire);
	}
private:
	Place* place;

	std::atomic<BaseItem*> items[BlockSize];
	std::atomic<size_t> offset;
	std::atomic<bool> reuse;

	std::atomic<Self*> next;
	Self* prev;
};

template <class DataBlock>
struct Strategy2BaseTaskStorageDataBlockReuseCheck {
	bool operator() (DataBlock const& db) const {
		return db.is_reusable();
	}
};

} /* namespace pheet */
#endif /* STRATEGY2BASETASKSTORAGEDATABLOCK_H_ */