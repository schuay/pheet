/*
 * LifoStrategy.h
 *
 *  Created on: 22.09.2011
 *      Author: mwimmer
 */

#ifndef LIFOSTRATEGY_H_
#define LIFOSTRATEGY_H_

#include "../BaseStrategy.h"
#include <iostream>

namespace pheet {

template <class Scheduler>
class LifoStrategy : public BaseStrategy<Scheduler> {
public:
	LifoStrategy();
	LifoStrategy(LifoStrategy& other);
	LifoStrategy(LifoStrategy&& other);
	virtual ~LifoStrategy();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc);

	static void print_name();
};

template <class Scheduler>
inline LifoStrategy<Scheduler>::LifoStrategy() {

}

template <class Scheduler>
inline LifoStrategy<Scheduler>::LifoStrategy(LifoStrategy& other) {

}

template <class Scheduler>
inline LifoStrategy<Scheduler>::LifoStrategy(LifoStrategy&& other) {

}

template <class Scheduler>
inline LifoStrategy<Scheduler>::~LifoStrategy() {

}

template <class Scheduler>
inline prio_t LifoStrategy<Scheduler>::get_pop_priority(size_t task_id) {
	return task_id + 1;
}

template <class Scheduler>
inline prio_t LifoStrategy<Scheduler>::get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc) {
	return task_id + 1;
}

template <class Scheduler>
inline void LifoStrategy<Scheduler>::print_name() {
	std::cout << "LifoStrategy";
}

}

#endif /* LIFOSTRATEGY_H_ */
