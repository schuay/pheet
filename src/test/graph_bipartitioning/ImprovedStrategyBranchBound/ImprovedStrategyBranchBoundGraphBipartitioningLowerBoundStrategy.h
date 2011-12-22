/*
 * ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy.h
 *
 *  Created on: Dec 21, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_

#include "../../../settings.h"
#include "../../../sched/strategies/UserDefinedPriority/UserDefinedPriority.h"

namespace pheet {

template <class Scheduler, class SubProblem>
class ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy();
	~ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy();

	UserDefinedPriority<Scheduler> operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
};

template <class Scheduler, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Scheduler, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy() {

}

template <class Scheduler, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Scheduler, SubProblem>::~ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy() {

}

template <class Scheduler, class SubProblem>
UserDefinedPriority<Scheduler> ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Scheduler, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	size_t lb = sub_problem->get_lower_bound();
	size_t ub = *upper_bound;

/*	prio_t prio_pop = 1 + depth * bound_diff;
	prio_t prio_steal = 1 + (sub_problem->size - depth) * bound_diff;*/

	return UserDefinedPriority<Scheduler>(ub - lb, ub - lb);
}

template <class Scheduler, class SubProblem>
inline void ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Scheduler, SubProblem>::print_name() {
	std::cout << "LowerBoundStrategy";
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_ */
