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
template <class Pheet, class Place, typename TT, size_t BlockSize>
class Strategy2BaseTaskStorageDataBlock {
public:
	typedef Strategy2BaseTaskStorageDataBlock<Pheet, TT, BlockSize> Self;
	typedef Strategy2BaseTaskStorageBaseItem<Pheet, TT> BaseItem;

	Strategy2BaseTaskStorageDataBlock()
	:reuse(true) {}
	~Strategy2BaseTaskStorageDataBlock() {}

	void link(Self* prev) {
		this->prev = prev;
		this->next.store(nullptr, std::memory_order_relaxed);
		if(prev != nullptr) {
			pheet_assert(prev->next == nullptr);
			offset = prev->offset + BlockSize;

			// When next pointer of predecessor is read (as not null), all initialization happened before
			prev->next.store(this, std::memory_order_release);
		}
		else {
			offset = 0;
		}
	}

	BaseItem* get(size_t pos) {
		pheet_assert(pos - offset < BlockSize);
		return items[pos - offset];
	}

	void put(BaseItem* item, size_t pos) {
		pheet_assert(pos - offset < BlockSize);
		items[pos - offset] = item;
	}

	bool fits(size_t pos) {
		return pos - offset < BlockSize;
	}

	int compare(size_t pos) {
		ptrdiff_t diff = (ptrdiff_t)(pos - offset);
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
		// Since reuse flag is the only thing written by foreign threads in a block,
		// no memory ordering constraints are needed
		reuse.store(true, std::memory_order_relaxed);
	}
private:
	Place* place;

	BaseItem* items[BlockSize];
	size_t offset;
	std::atomic<bool> reuse;

	std::atomic<Self*> next;
	Self* prev;
};

} /* namespace pheet */
#endif /* STRATEGY2BASETASKSTORAGEDATABLOCK_H_ */
