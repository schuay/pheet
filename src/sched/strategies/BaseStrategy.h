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

template <class Scheduler>
class BaseStrategy {
public:
	BaseStrategy();
	virtual ~BaseStrategy();

	virtual prio_t get_pop_priority(size_t task_id) = 0;
	virtual prio_t get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc) = 0;
};

template <class Scheduler>
inline BaseStrategy<Scheduler>::BaseStrategy() {

}

template <class Scheduler>
inline BaseStrategy<Scheduler>::~BaseStrategy() {

}

}

#endif /* BASESTRATEGY_H_ */
