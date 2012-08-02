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
  class NormalOrHighPrioStrategy :  public Pheet::Environment::BaseStrategy {

    bool highprio;
  public:

    typedef NormalOrHighPrioStrategy<Pheet> Self;
    typedef typename Pheet::Environment::BaseStrategy BaseStrategy;

  NormalOrHighPrioStrategy(bool highprio):highprio(highprio)
    {
      printf("U");
      this->set_transitive_weight(1);
    }

  NormalOrHighPrioStrategy():highprio(true) {}

    NormalOrHighPrioStrategy(Self& other)
      : BaseStrategy(other), highprio(other.highprio)
      {}

    NormalOrHighPrioStrategy(Self&& other)
      : BaseStrategy(other), highprio(other.highprio)
      {}

    ~NormalOrHighPrioStrategy() {}

    inline bool forbid_call_conversion() const {
      return false;
    }

    inline bool prioritize(Self& other) {
      printf("x\n");
      if(highprio!=other.highprio)
	return highprio;

      printf("k\n");
      bool res =  BaseStrategy::prioritize(other);
      printf("KalleC\n");
      return res;
    }
  };


template <class Pheet>
class SORLocalityStrategy :  public NormalOrHighPrioStrategy<Pheet>
  {
public:

	typedef SORLocalityStrategy<Pheet> Self;
	typedef NormalOrHighPrioStrategy<Pheet> BaseStrategy;

	SORLocalityStrategy(typename Pheet::Place* last_place, ptrdiff_t timestamp):last_place(last_place),timestamp(timestamp)
	{
//		this->set_memory_footprint(1);
		this->set_transitive_weight(1);
	}

	SORLocalityStrategy(Self& other)
	: BaseStrategy(other), last_place(other.last_place),timestamp(other.timestamp)
	{}

	SORLocalityStrategy(Self&& other)
	: BaseStrategy(other), last_place(other.last_place),timestamp(other.timestamp)
	{}

	~SORLocalityStrategy() {}

	inline bool forbid_call_conversion() const {
	  return true;
	}

	inline bool prioritize(Self& other) {

	  printf("y\n");

	  procs_t distancetothis = Pheet::get_place()->get_distance(last_place);
	  procs_t distancetoother = Pheet::get_place()->get_distance(other.last_place);

	  if(distancetothis != distancetoother)
	    return distancetothis < distancetoother;

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
	}

private:
	typename Pheet::Place* last_place;
	ptrdiff_t timestamp;
};


}

#endif /* SORLOCALITYSTRATEGY_H_ */
