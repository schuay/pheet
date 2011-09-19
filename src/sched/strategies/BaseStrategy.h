/*
 * BaseStrategy.h
 *
 *  Created on: 19.09.2011
 *      Author: mwimmer
 */

#ifndef BASESTRATEGY_H_
#define BASESTRATEGY_H_

namespace pheet {

typedef uint64_t prio_t;

template <class Sched>
class BaseStrategy {
public:
	typedef typename Scheduler::TaskExecutionContext TEC;

	BaseStrategy();
	virtual ~BaseStrategy();

	virtual prio_t get_pop_priority(TEC& tec) = 0;
	template <class HardwareType>
	virtual prio_t get_steal_priority<HardwareType>(TEC& tec, HardwareType) = 0;
};

inline BaseStrategy::BaseStrategy() {

}

inline BaseStrategy::~BaseStrategy() {

}

}

#endif /* BASESTRATEGY_H_ */
