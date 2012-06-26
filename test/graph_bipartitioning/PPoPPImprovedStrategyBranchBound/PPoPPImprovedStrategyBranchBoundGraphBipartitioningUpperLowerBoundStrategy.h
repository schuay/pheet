/*
 * ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy.h
 *
 *  Created on: Jan 13, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERLOWERBOUNDSTRATEGY_H_
#define PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERLOWERBOUNDSTRATEGY_H_

#include <pheet/pheet.h>
#include <pheet/sched/strategies/UserDefinedPriority/UserDefinedPriority.h>

namespace pheet {

template <class Pheet, class SubProblem>
  class PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy : public Pheet::Environment::BaseStrategy {
public:
    typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Pheet,SubProblem> Self;
    typedef typename Pheet::Environment::BaseStrategy BaseStrategy;
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy(SubProblem* sub_problem, size_t* upper_bound);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy(Self const& other);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy(Self && other);
    ~PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy();

    inline bool prioritize(Self& other);
    
    //	UserDefinedPriority<Pheet> operator()(SubProblem* sub_problem, size_t* upper_bound);
    
    static void print_name();
  private:
    SubProblem* sub_problem;
    size_t* upper_bound;

  };

template <class Pheet, class SubProblem>
  inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy(SubProblem* sub_problem, size_t* upper_bound): sub_problem(sub_problem),upper_bound(upper_bound) {

}


 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy(Self const& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }

 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy(Self&& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }



template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Pheet, SubProblem>::~PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy() {

}


 template <class Pheet, class SubProblem>
   inline bool PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Pheet, SubProblem>::prioritize(Self& other)
   {
     // TODO port strategy
     return BaseStrategy::prioritize(other);
   }

/*
template <class Pheet, class SubProblem>
UserDefinedPriority<Pheet> PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Pheet, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	size_t lb = sub_problem->get_lower_bound();
	size_t ub = sub_problem->get_upper_bound();

	pheet_assert(ub >= lb);

	// pop task with lowest upper bound (often depth first, but not in extreme cases
	// (upper bound is "best guaranteed upper bound for this subtree" doesn't mean
	// that all branches will be below it!)
	prio_t prio_pop = std::numeric_limits< prio_t >::max() - ub;

	// steal task with highest range of uncertainty
	prio_t prio_steal = std::numeric_limits< prio_t >::max() - lb;

	return UserDefinedPriority<Pheet>(prio_pop, prio_steal);
}
*/

template <class Pheet, class SubProblem>
inline void PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Pheet, SubProblem>::print_name() {
	std::cout << "UpperLowerBoundStrategy";
}

}
#endif /* PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERLOWERBOUNDSTRATEGY_H_ */
