/*
 * PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy.h
 *
 *  Created on: Dec 21, 2011
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_
#define PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_

#include <pheet/pheet.h>
#include <pheet/sched/strategies/UserDefinedPriority/UserDefinedPriority.h>

namespace pheet {

template <class Pheet, class SubProblem>
  class PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy : public Pheet::Environment::BaseStrategy {
  public:
    typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet,SubProblem> Self;
    typedef typename Pheet::Environment::BaseStrategy BaseStrategy;
    
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(SubProblem* sub_problem, size_t* upper_bound);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(Self const& other);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(Self && other);

	~PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy();

	inline bool prioritize(Self& other);

	//	UserDefinedPriority<Pheet> operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
  private:
        SubProblem* sub_problem;
        size_t* upper_bound;

};

template <class Pheet, class SubProblem>
  inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(SubProblem* sub_problem, size_t* upper_bound): sub_problem(sub_problem),upper_bound(upper_bound) {

}

 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(Self const& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }

 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(Self&& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }



template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::~PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy() {

}

 template <class Pheet, class SubProblem>
   inline bool PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::prioritize(Self& other)
   {
     // TODO port strategy
     return BaseStrategy::prioritize(other);
   }

/*
template <class Pheet, class SubProblem>
UserDefinedPriority<Pheet> PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	size_t lb = sub_problem->get_lower_bound();
	size_t ub = *upper_bound;

//	prio_t prio_pop = 1 + depth * bound_diff;
//	prio_t prio_steal = 1 + (sub_problem->size - depth) * bound_diff;

	return UserDefinedPriority<Pheet>(ub - lb, ub - lb);
	}*/

template <class Pheet, class SubProblem>
inline void PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::print_name() {
	std::cout << "LowerBoundStrategy";
}

}

#endif /* PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_ */
