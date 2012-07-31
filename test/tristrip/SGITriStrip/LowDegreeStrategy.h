/*
 * LowDegreeStrategy.h
 *
 *  Created on: Jan 4, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */
#ifndef LowDegreeSTRATEGY_H_
#define LowDegreeSTRATEGY_H_

#include <pheet/pheet.h>
#include <pheet/sched/strategies/BaseStrategy.h>

namespace pheet {
  
  template <class Pheet>
    class RunLastStrategy :  public Pheet::Environment::BaseStrategy {
  public:
    typedef RunLastStrategy<Pheet> Self;
    typedef typename Pheet::Environment::BaseStrategy BaseStrategy;
  protected:
    bool runlast;

  public:
  RunLastStrategy(bool runlast):runlast(runlast)
    {
    }
  RunLastStrategy():runlast(false)
    {
    }
    RunLastStrategy	(Self& other)
      : BaseStrategy(other),runlast(other.runlast)
      {
      }
    
  RunLastStrategy(Self&& other)
    : BaseStrategy(other), runlast(other.runlast)
      {
      }
    
    ~RunLastStrategy() {}
    
    inline bool prioritize(Self& other) {
      if(runlast != other.runlast)
	return !runlast;

      //  if(runlast == other.runlast)
	return BaseStrategy::prioritize(other);
    }
    
    inline bool forbid_call_conversion() const {
      return true;
    }
  };
  
  template <class Pheet>
    class LowDegreeStrategy : public  RunLastStrategy<Pheet> {
    //	GraphDual& graph;
    GraphNode* node;
    size_t degree;
    size_t taken;
  public:
    
    typedef LowDegreeStrategy<Pheet> Self;
    typedef RunLastStrategy<Pheet> Strategy;
   


  LowDegreeStrategy(/*GraphDual& graph,*/ GraphNode* node, size_t degree,size_t taken):/*graph(graph),*/node(node),degree(degree),taken(taken)
	{
	  Strategy::runlast = false;
//		this->set_memory_footprint(1);
	  degree = node->getExtendedDegree();
	  this->set_transitive_weight(100);
	}

	LowDegreeStrategy(Self& other)
	  : Strategy(other), /*graph(other.graph),*/node(other.node),degree(other.degree),taken(other.taken)
	{
	  degree = node->getExtendedDegree();
	  
}

	LowDegreeStrategy(Self&& other)
	  : Strategy(other), /*graph(other.graph),*/node(other.node),degree(other.degree),taken(other.taken)
	{
	  degree = node->getExtendedDegree();
}

	~LowDegreeStrategy() {}

	inline bool prioritize(Self& other) {
	  size_t thisd = degree;// node->getExtendedDegree();
	  size_t otherd = other.degree; //other.node->getExtendedDegree();

	  //	  printf("Degree %d vs %d\n",thisd,otherd);

	  if(thisd==otherd)
			return Strategy::prioritize(other);
			     		else
			return thisd < otherd;
//	  procs_t distancetothis = Pheet::get_place()->get_distance(last_place);
//	  procs_t distancetoother = Pheet::get_place()->get_distance(other.last_place);

	  //if(last_owner!=Pheet::get_place())
	  //	printf("Should be 6: %d\n",distancetothis);
	  //if(last_owner==Pheet::get_place())
	  //	printf("Should be 0: %d\n",distancetothis);

	  //printf("Distance1: %d Distance2: %d\n",distancetothis,distancetoother);

	//  if(distancetothis != distancetoother)
	  //  return distancetothis < distancetoother;

/*		if(this->get_place() != other.get_place())
		{
			if(this->get_place() == Pheet::get_place())
				return true;
			if(other.get_place() == Pheet::get_place())
				return false;
		}*/

/*	  if(distancetothis == 0)
	  {
	  	// If local task, prioritize recently used
	  	return timestamp > other.timestamp;
	  }
	  else
	  {
	  	// If stealing, prioritize not recently used
	  	return timestamp < other.timestamp;
	  }*/
	//	return BaseStrategy::prioritize(other);
	}

	inline bool forbid_call_conversion() const {
	  //  return true;
	  return false;
        }

};


}

#endif /* LowDegreeSTRATEGY_H_ */
