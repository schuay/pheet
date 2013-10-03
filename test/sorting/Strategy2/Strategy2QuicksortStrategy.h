/*
 * StrategyQuicksortStrategy.h
 *
 *  Created on: 03.04.2012
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef STRATEGY2QUICKSORTSTRATEGY_H_
#define STRATEGY2QUICKSORTSTRATEGY_H_

#include <pheet/misc/bitops.h>

#include <pheet/ds/StrategyTaskStorage/KLSMLocality/KLSMLocalityTaskStorage.h>

namespace pheet {

template <class Pheet>
class Strategy2QuicksortStrategy : public Pheet::Environment::BaseStrategy {
public:
	typedef Strategy2QuicksortStrategy<Pheet> Self;
	typedef typename Pheet::Environment::BaseStrategy BaseStrategy;

	typedef KLSMLocalityTaskStorage<Pheet, Self> TaskStorage;
	typedef typename TaskStorage::Place TaskStoragePlace;

	/*
	 * Default constructor is required by task storage
	 */
	Strategy2QuicksortStrategy() {

	}

	Strategy2QuicksortStrategy(size_t length)
	: length(length) {

	}

	Strategy2QuicksortStrategy(Self const& other) = default;

	Strategy2QuicksortStrategy(Self&& other) = default;

	~Strategy2QuicksortStrategy() {}

	Self& operator=(Self&& other) {
//		BaseStrategy::operator=(other);
		length = other.length;
		return *this;
	}

	bool prioritize(Self& other) {
		return length > other.length;
	}

	bool dead_task() {
		return false;
	}

	/*
	 * Checks whether spawn can be converted to a function call
	 */
	inline bool can_call(TaskStoragePlace*) {
		return false;
	}

private:
	size_t length;
};

}

#endif /* STRATEGYQUICKSORTSTRATEGY_H_ */
