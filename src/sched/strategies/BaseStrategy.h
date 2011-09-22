/*
 * BaseStrategy.h
 *
 *  Created on: 19.09.2011
 *      Author: mwimmer
 */

#ifndef BASESTRATEGY_H_
#define BASESTRATEGY_H_

#include "../../settings.h"
#include "../../misc/types.h"

namespace pheet {

typedef uint64_t prio_t;

class BaseStrategy {
public:
	BaseStrategy();
	virtual ~BaseStrategy();

	virtual prio_t get_pop_priority(size_t task_id) = 0;
//	template <class HardwareType>
	virtual prio_t get_steal_priority/*<HardwareType>*/(size_t task_id) = 0;
};

inline BaseStrategy::BaseStrategy() {

}

inline BaseStrategy::~BaseStrategy() {

}

}

#endif /* BASESTRATEGY_H_ */
