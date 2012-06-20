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
	{
		this->set_memory_footprint(1);
		this->set_transitive_weight(1);
	}

	SORLocalityStrategy(Self& other)
	: BaseStrategy(other), last_place(other.last_place)
	{}

	SORLocalityStrategy(Self&& other)
	: BaseStrategy(other), last_place(other.last_place)
	{}

	~SORLocalityStrategy() {}

	inline bool prioritize(Self& other) {

		if(this->get_place() != other.get_place())
		{
			if(this->get_place() == Pheet::get_place())
				return true;
			if(other.get_place() == Pheet::get_place())
				return false;
		}
		return BaseStrategy::prioritize(other);
	}

private:
	typename Pheet::Place* last_place;
};


}

#endif /* SORLOCALITYSTRATEGY_H_ */
