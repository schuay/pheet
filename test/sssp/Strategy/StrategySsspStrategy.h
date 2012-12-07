/*
 * StrategySsspStrategy.h
 *
 *  Created on: Jul 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef STRATEGYSSSPSTRATEGY_H_
#define STRATEGYSSSPSTRATEGY_H_

#include <limits>

namespace pheet {

template <class Pheet>
class StrategySsspStrategy : public Pheet::Environment::BaseStrategy {
public:
	typedef StrategySsspStrategy<Pheet> Self;
	typedef typename Pheet::Environment::BaseStrategy BaseStrategy;

	StrategySsspStrategy(size_t distance)
	: distance(distance), rnd(Pheet::rand_int((1 + distance) << 4)) {
		this->set_k(4);
	}

	StrategySsspStrategy(Self& other)
	: BaseStrategy(other), distance(other.distance), rnd(other.rnd) {

	}

	StrategySsspStrategy(Self&& other)
	: BaseStrategy(other), distance(other.distance), rnd(other.rnd) {}

	~StrategySsspStrategy() {}

	inline bool prioritize(Self& other) const {
		auto p = Pheet::get_place();
		if(this->get_place() == p) {
			if(other.get_place() == p) {
				return distance < other.distance;
			}
			return true;
		}
		else if(other.get_place() == p) {
			return false;
		}
		return rnd < other.rnd;

	//	return BaseStrategy::prioritize(other);
	}

	inline bool forbid_call_conversion() const {
		return true;
	}

	inline void rebase() {
		this->reset();
		rnd = Pheet::rand_int((1 + distance) << 4);
	}
private:
	size_t distance;
	size_t rnd;
};


} /* namespace pheet */
#endif /* STRATEGYSSSPSTRATEGY_H_ */
