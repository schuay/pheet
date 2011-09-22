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

namespace pheet {

class LifoFifoStrategy : public BaseStrategy {
public:
	LifoFifoStrategy();
	LifoFifoStrategy(LifoFifoStrategy& other);
	LifoFifoStrategy(LifoFifoStrategy&& other);
	virtual ~LifoFifoStrategy();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id);

private:
};

inline LifoFifoStrategy::LifoFifoStrategy() {

}

inline LifoFifoStrategy::LifoFifoStrategy(LifoFifoStrategy& other) {

}

inline LifoFifoStrategy::LifoFifoStrategy(LifoFifoStrategy&& other) {

}

inline LifoFifoStrategy::~LifoFifoStrategy() {

}

inline prio_t LifoFifoStrategy::get_pop_priority(size_t task_id) {
	return task_id + 1;
}

inline prio_t LifoFifoStrategy::get_steal_priority(size_t task_id) {
	return std::numeric_limits< prio_t >::max() - task_id;
}

}

#endif /* LIFOFIFOSTRATEGY_H_ */
