/*
 * UserDefinedPriority.h
 *
 *  Created on: 21.09.2011
 *      Author: mwimmer
 */

#ifndef USERDEFINEDPRIORITY_H_
#define USERDEFINEDPRIORITY_H_

#include "../BaseStrategy.h"

namespace pheet {

template <class Scheduler>
class UserDefinedPriority : public BaseStrategy<Scheduler> {
public:
	UserDefinedPriority(prio_t pop_priority, prio_t steal_priority);
	UserDefinedPriority(UserDefinedPriority& other);
	UserDefinedPriority(UserDefinedPriority&& other);
	virtual ~UserDefinedPriority();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc);
	virtual BaseStrategy<Scheduler>* clone();
private:
	prio_t pop_priority;
	prio_t steal_priority;
};

template <class Scheduler>
inline UserDefinedPriority<Scheduler>::UserDefinedPriority(prio_t pop_priority, prio_t steal_priority)
: pop_priority(pop_priority), steal_priority(steal_priority) {

}

template <class Scheduler>
inline UserDefinedPriority<Scheduler>::UserDefinedPriority(UserDefinedPriority& other)
: pop_priority(other.pop_priority), steal_priority(other.steal_priority) {

}

template <class Scheduler>
inline UserDefinedPriority<Scheduler>::UserDefinedPriority(UserDefinedPriority&& other)
: pop_priority(other.pop_priority), steal_priority(other.steal_priority) {

}

template <class Scheduler>
inline UserDefinedPriority<Scheduler>::~UserDefinedPriority() {

}

template <class Scheduler>
inline prio_t UserDefinedPriority<Scheduler>::get_pop_priority(size_t task_id) {
	return pop_priority;
}

template <class Scheduler>
inline prio_t UserDefinedPriority<Scheduler>::get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc) {
	return steal_priority;
}

template <class Scheduler>
inline BaseStrategy<Scheduler>* UserDefinedPriority<Scheduler>::clone() {
	return new UserDefinedPriority<Scheduler>(this);
}

}

#endif /* USERDEFINEDPRIORITY_H_ */
