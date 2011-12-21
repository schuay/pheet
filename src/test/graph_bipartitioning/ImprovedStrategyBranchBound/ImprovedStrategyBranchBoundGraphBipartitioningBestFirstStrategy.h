/*
 * ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy.h
 *
 *  Created on: Dec 5, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGBESTFIRSTSTRATEGY_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGBESTFIRSTSTRATEGY_H_

#include "../../../settings.h"
#include "../../../sched/strategies/UserDefinedPriority/UserDefinedPriority.h"

namespace pheet {

template <class SubProblem>
class ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy();
	~ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy();

	UserDefinedPriority operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
};

template <class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy<SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy() {

}

template <class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy<SubProblem>::~ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy() {

}

template <class SubProblem>
UserDefinedPriority ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy<SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	size_t ubc = *upper_bound;
	size_t lb = sub_problem->get_lower_bound();
	if(ubc < lb) {
		// Prioritize this task, as it will immediatly terminate anyway
		// Do not allow it to be stolen (not worth it)
		return UserDefinedPriority(std::numeric_limits< prio_t >::max(), 0);
	}

	// Depth: assumption: good for pop, bad for steal
	size_t depth = sub_problem->sets[0].count() + sub_problem->sets[1].count();

	// Remaining elements in sets. Presumably lb is more realistic in balanced sets?
	// (ignored for now)
//	size_t set1_remaining = k - set1_size;
//	size_t set2_remaining = (size - k) - set2_size;

	// Difference to upper bound - larger is better
	// One maybe good, maybe bad decision: Always use the value relative to the current upper bound
	// Why: gives older elements a slight advantage. They will be rechecked and maybe dropped altogether
	// Also problematic if upper bound is near the limits of size_t as we may have an overflow (shouldn't be a problem in our case)
	size_t bound_diff = ubc - lb;

	// Make sure we don't overflow
	size_t limit = (std::numeric_limits< prio_t >::max() - 1) / sub_problem->size;
	if(bound_diff > limit) {
		// No upper bound available yet
		bound_diff = limit;
	}

	prio_t prio_pop = 1 + depth * bound_diff;
	prio_t prio_steal = 1 + (sub_problem->size - depth) * bound_diff;

	return UserDefinedPriority(prio_pop, prio_steal);
}

template <class SubProblem>
inline void ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy<SubProblem>::print_name() {
	std::cout << "BestFirstStrategy";
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGBESTFIRSTSTRATEGY_H_ */
