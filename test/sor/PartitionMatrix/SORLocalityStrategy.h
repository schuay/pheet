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

	SORLocalityStrategy(typename Pheet::Place* last_place, ptrdiff_t timestamp):last_place(last_place),timestamp(timestamp)
	{
//		this->set_memory_footprint(1);
		this->set_transitive_weight(1000000);
	}

	SORLocalityStrategy(Self& other)
	: BaseStrategy(other), last_place(other.last_place),timestamp(other.timestamp)
	{}

	SORLocalityStrategy(Self&& other)
	: BaseStrategy(other), last_place(other.last_place),timestamp(other.timestamp)
	{}

	~SORLocalityStrategy() {}

	inline bool prioritize(Self& other) {
	  procs_t distancetothis = Pheet::get_place()->get_distance(last_place);
	  procs_t distancetoother = Pheet::get_place()->get_distance(other.last_place);

	  //if(last_owner!=Pheet::get_place())
	  //	printf("Should be 6: %d\n",distancetothis);
	  //if(last_owner==Pheet::get_place())
	  //	printf("Should be 0: %d\n",distancetothis);

	  //printf("Distance1: %d Distance2: %d\n",distancetothis,distancetoother);

	  if(distancetothis != distancetoother)
	    return distancetothis < distancetoother;

/*		if(this->get_place() != other.get_place())
		{
			if(this->get_place() == Pheet::get_place())
				return true;
			if(other.get_place() == Pheet::get_place())
				return false;
		}*/

	  if(distancetothis == 0)
	  {
	  	// If local task, prioritize recently used
	  	return timestamp > other.timestamp;
	  }
	  else
	  {
	  	// If stealing, prioritize not recently used
	  	return timestamp < other.timestamp;
	  }
		//return BaseStrategy::prioritize(other);
	}

private:
	typename Pheet::Place* last_place;
	ptrdiff_t timestamp;
};


}

#endif /* SORLOCALITYSTRATEGY_H_ */
