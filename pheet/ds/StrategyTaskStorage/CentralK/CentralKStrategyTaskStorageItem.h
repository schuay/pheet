/*
 * CentralKStrategyTaskStorageItem.h
 *
 *  Created on: 24.10.2012
 *      Author: mwimmer
 */

#ifndef CENTRALKSTRATEGYTASKSTORAGEITEM_H_
#define CENTRALKSTRATEGYTASKSTORAGEITEM_H_

namespace pheet {

template <class Pheet, class Place, typename TT>
struct CentralKStrategyTaskStorageItem {
	typedef CentralKStrategyTaskStorageItem<Pheet, Place, TT> Self;
	typedef TT Data;

	typename Pheet::Scheduler::BaseStrategy* strategy;
	TT data;
	size_t position;
	size_t orig_position;

	void (Place::*item_push)(Self* item, size_t position);
};

template <class Item>
struct CentralKStrategyTaskStorageItemReuseCheck {
	bool operator() (Item const& item) const {
		return item.position != item.orig_position && item.strategy == nullptr;
	}
};

} /* namespace pheet */
#endif /* CENTRALKSTRATEGYTASKSTORAGEITEM_H_ */
