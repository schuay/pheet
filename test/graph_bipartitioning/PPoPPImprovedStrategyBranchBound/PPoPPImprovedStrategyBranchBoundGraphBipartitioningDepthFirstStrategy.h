/*
 * ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy.h
 *
 *  Created on: Dec 21, 2011
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTSTRATEGY_H_
#define PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTSTRATEGY_H_

#include <pheet/pheet.h>
#include <pheet/sched/strategies/UserDefinedPriority/UserDefinedPriority.h>

namespace pheet {

template <class Pheet, class SubProblem>
  class PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy : public Pheet::Environment::BaseStrategy {
public:
    typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Pheet,SubProblem> Self;
    typedef typename  Pheet::Environment::BaseStrategy BaseStrategy;
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy(SubProblem* sub_problem, size_t* upper_bound);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy(Self const& other);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy(Self && other);

    ~PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy();

	inline bool prioritize(Self& other);

	//	UserDefinedPriority<Pheet> operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
  private:
        SubProblem* sub_problem;
        size_t* upper_bound;

};

template <class Pheet, class SubProblem>
  inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy(SubProblem* sub_problem, size_t* upper_bound): sub_problem(sub_problem),upper_bound(upper_bound) {

}


 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy(Self const& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }

 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy(Self&& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }



template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Pheet, SubProblem>::~PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy() {

}

 template <class Pheet, class SubProblem>
   inline bool PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Pheet, SubProblem>::prioritize(Self& other)
   {
     // TODO port strategy
     return BaseStrategy::prioritize(other);
   }

/*
template <class Pheet, class SubProblem>
UserDefinedPriority<Pheet> PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Pheet, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	// Depth: assumption: good for pop, bad for steal
	size_t depth = sub_problem->sets[0].count() + sub_problem->sets[1].count();

	prio_t prio_pop = 1 + depth;
	prio_t prio_steal = 1 + (sub_problem->size - depth);

	return UserDefinedPriority<Pheet>(prio_pop, prio_steal);
}
*/
template <class Pheet, class SubProblem>
inline void PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Pheet, SubProblem>::print_name() {
	std::cout << "DepthFirstStrategy";
}


}

#endif /* PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTSTRATEGY_H_ */
