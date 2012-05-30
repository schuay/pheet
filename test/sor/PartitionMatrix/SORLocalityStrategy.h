/*
 * SORLocalityStrategy.h
 *
 *  Created on: Jan 4, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef SORLOCALITYSTRATEGY_H_
#define SORLOCALITYSTRATEGY_H_

#include <pheet/pheet.h>
#include <pheet/sched/strategies/BaseStrategy.h>

namespace pheet {

template <class Pheet>
class SORLocalityStrategy :  public Pheet::Environment::BaseStrategy {
public:

	typedef SORLocalityStrategy<Pheet> Self;
	typedef typename Pheet::Environment::BaseStrategy BaseStrategy;

	// TODO replace with new
	SORLocalityStrategy(/*size_t depth*/)
	/*: depth(depth)*/ {
		//this->set_memory_footprint(length);
		//this->set_transitive_weight(length * find_last_bit_set(length));
	}

	// TODO replace with new
	SORLocalityStrategy(Self& other)
	: BaseStrategy(other)//, depth(other.depth)
	{}

	// TODO replace with new
	SORLocalityStrategy(Self&& other)
	: BaseStrategy(other)//, depth(other.depth)
	{}

	~SORLocalityStrategy() {}

	inline bool prioritize(Self& other) {
		// TODO replace with new
	/*	if(this->get_place() == other.get_place() && this->get_place() == Pheet::get_place())
			return this->depth < other.depth;
		else
			return this->depth > other.depth;*/
		return true;
	}

private:

	// TODO replace with new
	//	typename Scheduler::TaskExecutionContext* last_owner;
};


}

#endif /* SORLOCALITYSTRATEGY_H_ */
