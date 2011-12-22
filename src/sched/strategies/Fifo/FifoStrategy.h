/*
 * FifoStrategy.h
 *
 *  Created on: 22.09.2011
 *      Author: mwimmer
 */

#ifndef FIFOSTRATEGY_H_
#define FIFOSTRATEGY_H_

#include "../BaseStrategy.h"
#include <limits>
#include <iostream>

namespace pheet {

template <class Scheduler>
class FifoStrategy : public BaseStrategy<Scheduler> {
public:
	FifoStrategy();
	FifoStrategy(FifoStrategy& other);
	FifoStrategy(FifoStrategy&& other);
	virtual ~FifoStrategy();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc);

	static void print_name();
};

template <class Scheduler>
inline FifoStrategy<Scheduler>::FifoStrategy() {

}

template <class Scheduler>
inline FifoStrategy<Scheduler>::FifoStrategy(FifoStrategy& other) {

}

template <class Scheduler>
inline FifoStrategy<Scheduler>::FifoStrategy(FifoStrategy&& other) {

}

template <class Scheduler>
inline FifoStrategy<Scheduler>::~FifoStrategy() {

}

template <class Scheduler>
inline prio_t FifoStrategy<Scheduler>::get_pop_priority(size_t task_id) {
	return std::numeric_limits< prio_t >::max() - task_id;
}

template <class Scheduler>
inline prio_t FifoStrategy<Scheduler>::get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc) {
	return std::numeric_limits< prio_t >::max() - task_id;
}

template <class Scheduler>
inline void FifoStrategy<Scheduler>::print_name() {
	std::cout << "FifoStrategy";
}

}

#endif /* FIFOSTRATEGY_H_ */
