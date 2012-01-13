/*
 * ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy.h
 *
 *  Created on: Jan 13, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERLOWERBOUNDSTRATEGY_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERLOWERBOUNDSTRATEGY_H_

#include "../../../settings.h"
#include "../../../sched/strategies/UserDefinedPriority/UserDefinedPriority.h"

namespace pheet {

template <class Scheduler, class SubProblem>
class ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy();
	~ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy();

	UserDefinedPriority<Scheduler> operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
};

template <class Scheduler, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Scheduler, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy() {

}

template <class Scheduler, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Scheduler, SubProblem>::~ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy() {

}

template <class Scheduler, class SubProblem>
UserDefinedPriority<Scheduler> ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Scheduler, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	size_t lb = sub_problem->get_lower_bound();
	size_t ub = sub_problem->get_upper_bound();

	assert(ub >= lb);

	// pop task with lowest upper bound (often depth first, but not in extreme cases
	// (upper bound is "best guaranteed upper bound for this subtree" doesn't mean
	// that all branches will be below it!)
	prio_t prio_pop = std::numeric_limits< prio_t >::max() - ub;

	// steal task with highest range of uncertainty
	prio_t prio_steal = std::numeric_limits< prio_t >::max() - lb;

	return UserDefinedPriority<Scheduler>(prio_pop, prio_steal);
}

template <class Scheduler, class SubProblem>
inline void ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy<Scheduler, SubProblem>::print_name() {
	std::cout << "UpperLowerBoundStrategy";
}

}
#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERLOWERBOUNDSTRATEGY_H_ */
