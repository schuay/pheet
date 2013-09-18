/*
 * KLSMLocalityTaskStorageItem.h
 *
 *  Created on: Sep 18, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGEITEM_H_
#define KLSMLOCALITYTASKSTORAGEITEM_H_

namespace pheet {

template <class Pheet, class BaseItem, class Strategy>
struct KLSMLocalityTaskStorageItem : public BaseItem {
	Strategy strategy;
};

template <class Item>
struct KLSMLocalityTaskStorageItemReuseCheck {
	bool operator() (Item const& item) const {
		return item.taken;
	}
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEITEM_H_ */
