/*
 * PrioritySchedulerStealerDescriptor.h
 *
 *  Created on: Dec 22, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PRIORITYSCHEDULERSTEALERDESCRIPTOR_H_
#define PRIORITYSCHEDULERSTEALERDESCRIPTOR_H_

namespace pheet {

template <class Scheduler>
class PrioritySchedulerStealerDescriptor {
public:
	PrioritySchedulerStealerDescriptor();
	PrioritySchedulerStealerDescriptor(typename Scheduler::TaskExecutionContext* owner, typename Scheduler::TaskExecutionContext* stealer, procs_t max_granularity_level);
	~PrioritySchedulerStealerDescriptor();

	// Public interface for programmer
	procs_t get_distance();
	bool is_same_pu_type();

	// Interface for task data-structure
	void set_max_granularity_level(procs_t max);
	procs_t get_max_granularity_level();
private:
	typename Scheduler::TaskExecutionContext* owner;
	typename Scheduler::TaskExecutionContext* stealer;
	procs_t max_granularity_level;
};

/*
 * Creates empty descriptor with no granularity whatsoever
 */
template <class Scheduler>
PrioritySchedulerStealerDescriptor<Scheduler>::PrioritySchedulerStealerDescriptor()
: owner(Scheduler::get_context()), stealer(owner), max_granularity_level(0) {

}

template <class Scheduler>
PrioritySchedulerStealerDescriptor<Scheduler>::PrioritySchedulerStealerDescriptor(typename Scheduler::TaskExecutionContext* owner, typename Scheduler::TaskExecutionContext* stealer, procs_t max_granularity_level)
: owner(owner), stealer(stealer), max_granularity_level(max_granularity_level) {

}

template <class Scheduler>
PrioritySchedulerStealerDescriptor<Scheduler>::~PrioritySchedulerStealerDescriptor() {

}

template <class Scheduler>
inline procs_t PrioritySchedulerStealerDescriptor<Scheduler>::get_distance() {
	return owner->get_distance(stealer, max_granularity_level);
}

template <class Scheduler>
inline bool PrioritySchedulerStealerDescriptor<Scheduler>::is_same_pu_type() {
	// We are currently homogeneous, so we always have the same pu type
	return true;
}

template <class Scheduler>
inline void PrioritySchedulerStealerDescriptor<Scheduler>::set_max_granularity_level(procs_t max) {
	max_granularity_level = max;
}

template <class Scheduler>
inline procs_t PrioritySchedulerStealerDescriptor<Scheduler>::get_max_granularity_level() {
	return max_granularity_level;
}

}

#endif /* PRIORITYSCHEDULERSTEALERDESCRIPTOR_H_ */
