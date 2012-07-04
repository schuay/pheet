/*
 * StrategyRecursiveParallelPrefixSumStrategy.h
 *
 *  Created on: Jul 4, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef STRATEGYRECURSIVEPARALLELPREFIXSUMSTRATEGY_H_
#define STRATEGYRECURSIVEPARALLELPREFIXSUMSTRATEGY_H_

namespace pheet {

template <class Pheet>
class StrategyRecursiveParallelPrefixSumStrategy : public Pheet::Environment::BaseStrategy {
public:
	typedef StrategyRecursiveParallelPrefixSumStrategy<Pheet> Self;
	typedef typename Pheet::Environment::BaseStrategy BaseStrategy;

	StrategyRecursiveParallelPrefixSumStrategy(unsigned int* data, size_t length, bool in_order, bool no_call)
	:data(data), length(length), in_order(in_order), no_call(no_call) {
		this->set_transitive_weight(length >> 9);
	}
	~StrategyRecursiveParallelPrefixSumStrategy() {}

	inline bool prioritize(Self& other) {
		if(this->get_place() == Pheet::get_place()) {
			if(this->get_place() == other.get_place()) {
				// Both locally spawned.

				if(in_order) {
					if(other.in_order) {
						pheet_assert(this == &other || data != other.data);
						return data < other.data;
					}
					return true;
				}
				else if(other.in_order) {
					return false;
				}
				return length > other.length;
			}
			return true;
		}
		else if(other.get_place() == Pheet::get_place()) {
			return false;
		}

		if(in_order != other.in_order) {
			return other.in_order;
		}
		return data > other.data;
	}

	inline bool forbid_call_conversion() {
		return true; //no_call;
	}

private:
	unsigned int* data;
	size_t length;
	bool in_order;
	bool no_call;
};

} /* namespace pheet */
#endif /* STRATEGYRECURSIVEPARALLELPREFIXSUMSTRATEGY_H_ */
