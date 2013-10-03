/*
 * KLSMLocalityTaskStorageItem.h
 *
 *  Created on: Sep 18, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGEITEM_H_
#define KLSMLOCALITYTASKSTORAGEITEM_H_

#include <atomic>

namespace pheet {

template <class Pheet, class Place, class Frame, class FrameReg, class BaseItem, class Strategy>
struct KLSMLocalityTaskStorageItem : public BaseItem {
	typedef typename BaseItem::T T;

	KLSMLocalityTaskStorageItem()
	:owner(nullptr), used_locally(false), frame(nullptr), last_phase(0) {
	}

	/*
	 * Is only safe to be called if the thread owns the item or is registered to the frame
	 */
	bool is_taken() {
		return last_phase.load(std::memory_order_acquire) != -1;
	}

	/*
	 * Is only safe to be called if the thread owns the item or is registered to the frame
	 */
	bool is_taken_or_dead() {
		return last_phase.load(std::memory_order_acquire) != -1 || strategy.dead_task();
	}

	/*
	 * Local version of take, called by owner
	 */
	T take(Place* place) {
		ptrdiff_t expected = -1;
		if(last_phase.compare_exchange_strong(expected, frame.load(std::memory_order_relaxed)->get_phase(), std::memory_order_release, std::memory_order_acquire)) {
			pheet_assert(used_locally);
			this->taken.store(true, std::memory_order_relaxed);
			used_locally = false;
			return this->data;
		}
		used_locally = false;
		return nullable_traits<T>::null_value;
	}

	/*
	 * Remote version of take, called by all threads except the owner
	 */
	T take(FrameReg& reg) {
		ptrdiff_t expected = -1;
		Frame* f = frame.load(std::memory_order_relaxed);
		if(last_phase.compare_exchange_strong(expected, reg.get_phase(), std::memory_order_release, std::memory_order_acquire)) {
			this->taken.store(true, std::memory_order_relaxed);
			T data = this->data;

			reg.rem_ref(f);
			return data;
		}
		reg.rem_ref(f);
		return nullable_traits<T>::null_value;
	}

	/*
	 * Local version of take_and_delete, called by owner
	 * Tries to take item and deletes it on success
	 */
	void take_and_delete(Place* place) {
		ptrdiff_t expected = -1;
		if(last_phase.compare_exchange_strong(expected, frame.load(std::memory_order_relaxed)->get_phase(), std::memory_order_release, std::memory_order_acquire)) {
			this->taken.store(true, std::memory_order_relaxed);

			this->data.drop_item();
		}
		pheet_assert(used_locally);
		used_locally = false;
	}

	/*
	 * Remote version of take_and_delete, called by all threads except the owner
	 * Tries to take item and deletes it on success
	 */
	void take_and_delete(FrameReg& reg) {
		ptrdiff_t expected = -1;
		Frame* f = frame.load(std::memory_order_relaxed);
		if(last_phase.compare_exchange_strong(expected, reg.get_phase(), std::memory_order_release, std::memory_order_acquire)) {
			this->taken.store(true, std::memory_order_relaxed);

			this->data.drop_item();
		}
		reg.rem_ref(f);
	}

	Place* owner;
	Strategy strategy;
	bool used_locally;

	std::atomic<Frame*> frame;
	std::atomic<ptrdiff_t> last_phase;
};

template <class Item, class Frame>
struct KLSMLocalityTaskStorageItemReuseCheck {
	bool operator() (Item const& item) const {
		Frame* f = item.frame.load(std::memory_order_relaxed);
		// item.taken is always set after last_phase is set, therefore use this for checks
		return item.taken && !item.used_locally && (f == nullptr || f->can_reuse(item.last_phase.load(std::memory_order_relaxed)));
	}
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEITEM_H_ */
