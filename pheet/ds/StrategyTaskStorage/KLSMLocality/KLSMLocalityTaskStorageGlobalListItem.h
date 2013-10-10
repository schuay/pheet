/*
 * KLSMLocalityTaskStorageGlobalListItem.h
 *
 *  Created on: Oct 9, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGEGLOBALLISTITEM_H_
#define KLSMLOCALITYTASKSTORAGEGLOBALLISTITEM_H_

#include <atomic>

namespace pheet {

template <class Pheet, class Block>
class KLSMLocalityTaskStorageGlobalListItem {
public:
	typedef KLSMLocalityTaskStorageGlobalListItem<Pheet, Block> Self;
	KLSMLocalityTaskStorageGlobalListItem()
	:block(nullptr), registered(0), next(this) {

	}
	~KLSMLocalityTaskStorageGlobalListItem() {

	}

	void reset() {
		pheet_assert(is_reusable());
//		block.store(nullptr, std::memory_order_relaxed);
		// The thread linking the new item does not decrement registered, so we only need P-1
		registered.store(Pheet::get_num_places() - 1, std::memory_order_relaxed);
		next.store(nullptr, std::memory_order_relaxed);
	}

	Self* move_next() {
		Self* ret = next.load(std::memory_order_relaxed);
		if(ret != nullptr) {
			pheet_assert(registered.load(std::memory_order_relaxed) != 0);
			registered.fetch_sub(1, std::memory_order_relaxed);
		}
		return ret;
	}

	Block* get_block() {
		return block.load(std::memory_order_relaxed);
	}

	Block* acquire_block() {
		return block.load(std::memory_order_acquire);
	}

	bool is_reusable() const {
		// TODO: switch to more sophisticated scheme where some empty items can be reused
		// even if not seen by all threads
		return registered.load(std::memory_order_relaxed) == 0 && next.load(std::memory_order_relaxed) != nullptr;
	}

	void update_block(Block* b) {
		block.store(b, std::memory_order_release);
	}

	bool link(Self* i) {
		Self* n = next.load(std::memory_order_relaxed);
		if(n == nullptr) {
			if(next.compare_exchange_strong(n, i, std::memory_order_release, std::memory_order_acquire)) {
				return true;
			}
		}
		return false;
	}

	void local_link(Self* i) {
		pheet_assert(next.load(std::memory_order_relaxed) == nullptr);
		next.store(i, std::memory_order_relaxed);
	}

private:
	std::atomic<Block*> block;
	std::atomic<s_procs_t> registered;
	std::atomic<Self*> next;
};


template <class Item>
struct KLSMLocalityTaskStorageGlobalListItemReuseCheck {
	bool operator() (Item const& item) const {
		return item.is_reusable();
	}
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEGLOBALLISTITEM_H_ */
