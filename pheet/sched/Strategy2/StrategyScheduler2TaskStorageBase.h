/*
 * StrategyScheduler2TaskStorageBase.h
 *
 *  Created on: Aug 12, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef STRATEGYSCHEDULER2TASKSTORAGEBASE_H_
#define STRATEGYSCHEDULER2TASKSTORAGEBASE_H_

#include "StrategyScheduler2TaskStorageItem.h"

namespace pheet {

template <class Pheet, typename BaseItem, typename TT>
class StrategyScheduler2TaskStorageBase {
public:
	StrategyScheduler2TaskStorageBase();
	virtual ~StrategyScheduler2TaskStorageBase();

	/**
	 * Needs to contain at least one release statement or fence on success
	 * to make sure the parent task storage is correct. Release has to occur at latest at
	 * linearization point of success
	 * No requirements for failure
	 */
	virtual TT pop(BaseItem* boundary, procs_t place_id) = 0;
};

} /* namespace pheet */
#endif /* STRATEGYSCHEDULER2TASKSTORAGEBASE_H_ */
