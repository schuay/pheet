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

	virtual prio_t get_pop_priority();
	virtual prio_t get_steal_priority();

private:
	static thread_local prio_t offset;

	prio_t prio;
};

inline LifoFifoStrategy::LifoFifoStrategy()
: prio(++offset) {

}

inline LifoFifoStrategy::LifoFifoStrategy(LifoFifoStrategy& other)
: prio(other.prio) {

}

inline LifoFifoStrategy::LifoFifoStrategy(LifoFifoStrategy&& other)
: prio(other.prio) {

}

inline LifoFifoStrategy::~LifoFifoStrategy() {

}

inline prio_t LifoFifoStrategy::get_pop_priority() {
	return prio;
}

inline prio_t LifoFifoStrategy::get_steal_priority() {
	return std::numeric_limits< prio_t >::max() - offset;
}

}

#endif /* LIFOFIFOSTRATEGY_H_ */
