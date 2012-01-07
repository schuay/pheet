/*
 * LifoFifoStrategy.h
 *
 *  Created on: 21.09.2011
 *      Author: mwimmer
 */

#ifndef LIFOFIFOSTRATEGY_H_
#define LIFOFIFOSTRATEGY_H_

#include "../BaseStrategy.h"
#include <limits>
#include <iostream>

namespace pheet {

template <class Scheduler>
class LifoFifoStrategy : public BaseStrategy<Scheduler> {
public:
	LifoFifoStrategy();
	LifoFifoStrategy(LifoFifoStrategy& other);
	LifoFifoStrategy(LifoFifoStrategy&& other);
	virtual ~LifoFifoStrategy();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc);
	virtual BaseStrategy<Scheduler>* clone();

	static void print_name();

private:
};

template <class Scheduler>
inline LifoFifoStrategy<Scheduler>::LifoFifoStrategy() {

}

template <class Scheduler>
inline LifoFifoStrategy<Scheduler>::LifoFifoStrategy(LifoFifoStrategy& other) {

}

template <class Scheduler>
inline LifoFifoStrategy<Scheduler>::LifoFifoStrategy(LifoFifoStrategy&& other) {

}

template <class Scheduler>
inline LifoFifoStrategy<Scheduler>::~LifoFifoStrategy() {

}

template <class Scheduler>
inline prio_t LifoFifoStrategy<Scheduler>::get_pop_priority(size_t task_id) {
	return task_id + 1;
}

template <class Scheduler>
inline prio_t LifoFifoStrategy<Scheduler>::get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc) {
	return std::numeric_limits< prio_t >::max() - task_id;
}

template <class Scheduler>
inline BaseStrategy<Scheduler>* LifoFifoStrategy<Scheduler>::clone() {
	return new LifoFifoStrategy<Scheduler>(this);
}

template <class Scheduler>
inline void LifoFifoStrategy<Scheduler>::print_name() {
	std::cout << "LifoFifoStrategy";
}

}

#endif /* LIFOFIFOSTRATEGY_H_ */
