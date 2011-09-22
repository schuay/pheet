/*
 * LifoStrategy.h
 *
 *  Created on: 22.09.2011
 *      Author: mwimmer
 */

#ifndef LIFOSTRATEGY_H_
#define LIFOSTRATEGY_H_

#include "../BaseStrategy.h"

namespace pheet {

class LifoStrategy : public BaseStrategy {
public:
	LifoStrategy();
	LifoStrategy(LifoStrategy& other);
	LifoStrategy(LifoStrategy&& other);
	virtual ~LifoStrategy();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id);
};

inline LifoStrategy::LifoStrategy() {

}

inline LifoStrategy::LifoStrategy(LifoStrategy& other) {

}

inline LifoStrategy::LifoStrategy(LifoStrategy& other) {

}

inline LifoStrategy::~LifoStrategy() {

}

inline prio_t LifoStrategy::get_pop_priority(size_t task_id) {
	return task_id + 1;
}

inline prio_t LifoStrategy::get_steal_priority(size_t task_id) {
	return task_id + 1;
}

}

#endif /* LIFOSTRATEGY_H_ */
