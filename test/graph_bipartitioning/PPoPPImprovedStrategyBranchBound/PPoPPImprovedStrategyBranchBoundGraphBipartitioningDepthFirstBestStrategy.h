/*
 * ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy.h
 *
 *  Created on: Jan 12, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTBESTSTRATEGY_H_
#define PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTBESTSTRATEGY_H_

#include "pheet/pheet.h"
#include "pheet/sched/strategies/UserDefinedPriority/UserDefinedPriority.h"

namespace pheet {

template <class Pheet, class SubProblem>
  class PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy : public Pheet::Environment::BaseStrategy {
public:
    typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy<Pheet,SubProblem> Self;
    typedef typename Pheet::Environment::BaseStrategy BaseStrategy;

    PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy(SubProblem* sub_problem, size_t* upper_bound);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy(Self const& other);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy(Self && other);

	~PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy();

	//	UserDefinedPriority<Pheet> operator()(SubProblem* sub_problem, size_t* upper_bound);

	inline bool prioritize(Self& other);


	static void print_name();

  private:
	SubProblem* sub_problem;
	size_t* upper_bound;

};

template <class Pheet, class SubProblem>
  inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy(SubProblem* sub_problem, size_t* upper_bound): sub_problem(sub_problem),upper_bound(upper_bound) {

}


 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy(Self const& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }

 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy(Self&& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }



template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy<Pheet, SubProblem>::~PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy() {

}

 template <class Pheet, class SubProblem>
   inline bool PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy<Pheet, SubProblem>::prioritize(Self& other)
   {
     // TODO fix proper strategy
     return BaseStrategy::prioritize(other);
   }

 /*

template <class Pheet, class SubProblem>
UserDefinedPriority<Pheet> PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy<Pheet, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	size_t ubc = *upper_bound;
	size_t lb = sub_problem->get_lower_bound();
	if(ubc < lb) {
		// Prioritize this task, as it will immediatly terminate anyway
		// Do not allow it to be stolen (not worth it)
		return UserDefinedPriority<Pheet>(std::numeric_limits< prio_t >::max(), 0);
	}

	// Depth: assumption: good for pop, bad for steal
	size_t depth = sub_problem->sets[0].count() + sub_problem->sets[1].count();
	size_t max_depth = sub_problem->size;

	prio_t prio_pop = (std::numeric_limits< prio_t >::max() / (max_depth + 1)) * depth - lb;
	prio_t prio_steal = (std::numeric_limits< prio_t >::max() / (max_depth + 1)) * (max_depth - depth) - lb;

	return UserDefinedPriority<Pheet>(prio_pop, prio_steal);
}

 */

template <class Pheet, class SubProblem>
inline void PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy<Pheet, SubProblem>::print_name() {
	std::cout << "DepthFirstBestStrategy";
}

}

#endif /* PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTBESTSTRATEGY_H_ */
