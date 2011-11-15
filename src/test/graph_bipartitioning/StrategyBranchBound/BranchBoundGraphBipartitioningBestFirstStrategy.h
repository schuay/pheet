/*
 * BranchBoundGraphBipartitioningBestFirstStrategy.h
 *
 *  Created on: Nov 15, 2011
 *      Author: mwimmer
 */

#ifndef BRANCHBOUNDGRAPHBIPARTITIONINGBESTFIRSTSTRATEGY_H_
#define BRANCHBOUNDGRAPHBIPARTITIONINGBESTFIRSTSTRATEGY_H_

#include "../../../settings.h"
#include "../../../sched/strategies/UserDefinedPriority/UserDefinedPriority.h"

namespace pheet {

class BranchBoundGraphBipartitioningBestFirstStrategy {
public:
	BranchBoundGraphBipartitioningBestFirstStrategy();
	~BranchBoundGraphBipartitioningBestFirstStrategy();

	UserDefinedPriority operator()(GraphVertex* graph, size_t size, size_t k, size_t* set1, size_t set1_size, size_t* set2, size_t set2_size, size_t* ub, size_t lb);

	static void print_name();
};

inline BranchBoundGraphBipartitioningBestFirstStrategy::BranchBoundGraphBipartitioningBestFirstStrategy() {

}

inline BranchBoundGraphBipartitioningBestFirstStrategy::~BranchBoundGraphBipartitioningBestFirstStrategy() {

}

inline UserDefinedPriority BranchBoundGraphBipartitioningBestFirstStrategy::operator()(GraphVertex* graph, size_t size, size_t k, size_t* set1, size_t set1_size, size_t* set2, size_t set2_size, size_t* ub, size_t lb) {
	size_t ubc = *ub;
	if(ubc < lb) {
		// Prioritize this task, as it will immediatly terminate anyway
		// Do not allow it to be stolen (not worth it)
		return UserDefinedPriority(std::numeric_limits< prio_t >::max(), 0);
	}

	// Depth: assumption: good for pop, bad for steal
	size_t depth = set1_size + set2_size;

	// Remaining elements in sets. Presumably lb is more realistic in balanced sets?
	// (ignored for now)
//	size_t set1_remaining = k - set1_size;
//	size_t set2_remaining = (size - k) - set2_size;

	// Difference to upper bound - larger is better
	// One maybe good, maybe bad decision: Always use the value relative to the current upper bound
	// Why: gives older elements a slight advantage. They will be rechecked and maybe dropped altogether
	size_t bound_diff = ubc - lb;

	prio_t prio_pop = 1 + depth * bound_diff;
	prio_t prio_steal = 1 + (size - depth) * bound_diff;

	return UserDefinedPriority(prio_pop, prio_steal);
}

inline void BranchBoundGraphBipartitioningBestFirstStrategy::print_name() {
	std::cout << "BestFirstStrategy";
}

}

#endif /* BRANCHBOUNDGRAPHBIPARTITIONINGBESTFIRSTSTRATEGY_H_ */
