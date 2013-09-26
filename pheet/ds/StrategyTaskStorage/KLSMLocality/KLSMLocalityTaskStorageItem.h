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

template <class Pheet, class Place, class Frame, class BaseItem, class Strategy>
struct KLSMLocalityTaskStorageItem : public BaseItem {
	KLSMLocalityTaskStorageItem()
	:owner(Pheet::get_place()), used_locally(false), frame(nullptr), last_phase(-1) {
		owner = Pheet::get_place();
	}

	Place* owner;
	Strategy strategy;
	bool used_locally;

	std::atomic<Frame*> frame;
	std::atomic<ptrdiff_t> last_phase;
};

template <class Item>
struct KLSMLocalityTaskStorageItemReuseCheck {
	bool operator() (Item const& item) const {
		Frame* f = frame.load(std::memory_order_relaxed);
		// item.taken is always set after last_phase is set, therefore use this for checks
		return item.taken && !used_locally && (f == nullptr || f->can_reuse(item.last_phase.load(std::memory_order_relaxed)));
	}
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEITEM_H_ */
