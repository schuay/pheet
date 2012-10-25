/*
 * CentralKStrategyTaskStorageItem.h
 *
 *  Created on: 24.10.2012
 *      Author: mwimmer
 */

#ifndef CENTRALKSTRATEGYTASKSTORAGEITEM_H_
#define CENTRALKSTRATEGYTASKSTORAGEITEM_H_

namespace pheet {

template <class Pheet, typename TT>
struct CentralKStrategyTaskStorageItem {
	typedef CentralKStrategyTaskStorageItem<Pheet, TT> Self;
	typedef TT Data;

	typename Pheet::Scheduler::BaseStrategy* strategy;
	TT data;
	size_t position;
	Block* block;

	void (Place::*item_push)(Self* item);
};

} /* namespace pheet */
#endif /* CENTRALKSTRATEGYTASKSTORAGEITEM_H_ */
