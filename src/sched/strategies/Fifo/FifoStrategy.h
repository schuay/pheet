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

namespace pheet {

class FifoStrategy : public BaseStrategy {
public:
	FifoStrategy();
	FifoStrategy(FifoStrategy& other);
	FifoStrategy(FifoStrategy&& other);
	virtual ~FifoStrategy();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id);
};

inline FifoStrategy::FifoStrategy() {

}

inline FifoStrategy::FifoStrategy(FifoStrategy& other) {

}

inline FifoStrategy::FifoStrategy(FifoStrategy& other) {

}

inline FifoStrategy::~FifoStrategy() {

}

inline prio_t FifoStrategy::get_pop_priority(size_t task_id) {
	return std::numeric_limits< prio_t >::max() - task_id;
}

inline prio_t FifoStrategy::get_steal_priority(size_t task_id) {
	return std::numeric_limits< prio_t >::max() - task_id;
}

}

#endif /* FIFOSTRATEGY_H_ */
