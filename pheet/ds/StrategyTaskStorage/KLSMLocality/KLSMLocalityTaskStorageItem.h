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
	Place* owner;
	Strategy strategy;
	bool used_locally;

	Frame* frame;
	std::atomic<size_t> last_phase;
};

template <class Item>
struct KLSMLocalityTaskStorageItemReuseCheck {
	bool operator() (Item const& item) const {
		// item.taken is always set after last_phase is set, therefore use this for checks
		return item.taken/* && frame->can_reuse(last_phase)*/;
	}
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEITEM_H_ */
