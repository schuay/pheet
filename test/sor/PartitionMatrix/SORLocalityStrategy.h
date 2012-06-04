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

	SORLocalityStrategy(typename Pheet::Place* last_place):last_place(last_place)
	/*: depth(depth)*/ {
		//this->set_memory_footprint(length);
		//this->set_transitive_weight(length * find_last_bit_set(length));
	}

	SORLocalityStrategy(Self& other)
	: BaseStrategy(other), last_place(other.last_place)
	{}

	SORLocalityStrategy(Self&& other)
	: BaseStrategy(other), last_place(other.last_place)
	{}

	~SORLocalityStrategy() {}

	inline bool prioritize(Self& other) {

		return ((last_place)==other.get_place());


		// TODO replace with new
	/*	if(this->get_place() == other.get_place() && this->get_place() == Pheet::get_place())
			return this->depth < other.depth;
		else
			return this->depth > other.depth;*/
		return true;
	}

private:

	typename Pheet::Place* last_place;
};


}

#endif /* SORLOCALITYSTRATEGY_H_ */
