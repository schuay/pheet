/*
 * ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy.h
 *
 *  Created on: Dec 21, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTSTRATEGY_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTSTRATEGY_H_

#include "../../../settings.h"
#include "../../../sched/strategies/UserDefinedPriority/UserDefinedPriority.h"

namespace pheet {

template <class Scheduler, class SubProblem>
class ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy();
	~ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy();

	UserDefinedPriority<Scheduler> operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
};

template <class Scheduler, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Scheduler, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy() {

}

template <class Scheduler, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Scheduler, SubProblem>::~ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy() {

}

template <class Scheduler, class SubProblem>
UserDefinedPriority<Scheduler> ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Scheduler, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	// Depth: assumption: good for pop, bad for steal
	size_t depth = sub_problem->sets[0].count() + sub_problem->sets[1].count();

	prio_t prio_pop = 1 + depth;
	prio_t prio_steal = 1 + (sub_problem->size - depth);

	return UserDefinedPriority<Scheduler>(prio_pop, prio_steal);
}

template <class Scheduler, class SubProblem>
inline void ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy<Scheduler, SubProblem>::print_name() {
	std::cout << "DepthFirstStrategy";
}


}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDEPTHFIRSTSTRATEGY_H_ */
