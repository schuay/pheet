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

class UserDefinedPriority : public BaseStrategy {
public:
	UserDefinedPriority(prio_t pop_priority, prio_t steal_priority);
	UserDefinedPriority(UserDefinedPriority& other);
	UserDefinedPriority(UserDefinedPriority&& other);
	virtual ~UserDefinedPriority();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id);
private:
	prio_t pop_priority;
	prio_t steal_priority;
};

inline UserDefinedPriority::UserDefinedPriority(prio_t pop_priority, prio_t steal_priority)
: pop_priority(pop_priority), steal_priority(steal_priority) {

}

inline UserDefinedPriority::UserDefinedPriority(UserDefinedPriority& other)
: pop_priority(other.pop_priority), steal_priority(other.steal_priority) {

}

inline UserDefinedPriority::UserDefinedPriority(UserDefinedPriority&& other)
: pop_priority(other.pop_priority), steal_priority(other.steal_priority) {

}

inline UserDefinedPriority::~UserDefinedPriority() {

}

inline prio_t get_pop_priority(size_t task_id) {
	return pop_priority;
}

inline prio_t get_steal_priority(size_t task_id) {
	return steal_priority;
}

#endif /* USERDEFINEDPRIORITY_H_ */
