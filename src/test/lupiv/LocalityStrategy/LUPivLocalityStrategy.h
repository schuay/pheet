/*
 * LUPivLocalityStrategy.h
 *
 *  Created on: Jan 4, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LUPIVLOCALITYSTRATEGY_H_
#define LUPIVLOCALITYSTRATEGY_H_

#include "../../../settings.h"
#include "../../../sched/strategies/BaseStrategy.h"

namespace pheet {

template <class Scheduler>
class LUPivLocalityStrategy : public BaseStrategy<Scheduler> {
public:
	LUPivLocalityStrategy(typename Scheduler::TaskExecutionContext* last_owner, prio_t base_pop_priority, prio_t base_steal_priority);
	LUPivLocalityStrategy(LUPivLocalityStrategy& other);
	LUPivLocalityStrategy(LUPivLocalityStrategy&& other);
	virtual ~LUPivLocalityStrategy();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc);
	virtual BaseStrategy<Scheduler>* clone();

private:
	typename Scheduler::TaskExecutionContext* last_owner;
	prio_t base_pop_priority;
	prio_t base_steal_priority;
};

template <class Scheduler>
LUPivLocalityStrategy<Scheduler>::LUPivLocalityStrategy(typename Scheduler::TaskExecutionContext* last_owner, prio_t base_pop_priority, prio_t base_steal_priority)
: last_owner(last_owner), base_pop_priority(base_pop_priority), base_steal_priority(base_steal_priority) {
	assert(base_pop_priority < 16);
	assert(base_steal_priority < 16);
}

template <class Scheduler>
LUPivLocalityStrategy<Scheduler>::LUPivLocalityStrategy(LUPivLocalityStrategy& other)
: last_owner(other.last_owner), base_pop_priority(other.base_pop_priority), base_steal_priority(other.base_steal_priority) {

}

template <class Scheduler>
LUPivLocalityStrategy<Scheduler>::LUPivLocalityStrategy(LUPivLocalityStrategy&& other)
: last_owner(other.last_owner), base_pop_priority(other.base_pop_priority), base_steal_priority(other.base_steal_priority) {

}

template <class Scheduler>
LUPivLocalityStrategy<Scheduler>::~LUPivLocalityStrategy() {

}

template <class Scheduler>
inline prio_t LUPivLocalityStrategy<Scheduler>::get_pop_priority(size_t task_id) {
	typename Scheduler::TaskExecutionContext* owner = Scheduler::get_context();
	procs_t max_d = owner->get_max_distance();
	procs_t d = owner->get_distance(last_owner);
	return ((max_d - d) << 4) + base_pop_priority;
}

template <class Scheduler>
inline prio_t LUPivLocalityStrategy<Scheduler>::get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc) {
	procs_t max_d = desc.get_max_distance();
	procs_t d = desc.get_distance_to(last_owner);
	return ((max_d - d) << 4) + base_steal_priority;
}

template <class Scheduler>
inline BaseStrategy<Scheduler>* LUPivLocalityStrategy<Scheduler>::clone() {
	return new LUPivLocalityStrategy<Scheduler>(*this);
}

}

#endif /* LUPIVLOCALITYSTRATEGY_H_ */
