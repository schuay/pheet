/*
 * ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy.h
 *
 *  Created on: Dec 5, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGAUTOSTRATEGY_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGAUTOSTRATEGY_H_

#include "../../../settings.h"
#include "../ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningSubproblem.h"

namespace pheet {

template <class Strategy>
class ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy();
	~ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy();

	Strategy operator()(ImprovedBranchBoundGraphBipartitioningSubproblem* sub_problem, size_t* upper_bound);

	static void print_name();
};

template <class Strategy>
inline ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Strategy>::ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy() {

}

template <class Strategy>
inline ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Strategy>::~ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy() {

}

template <class Strategy>
inline Strategy ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Strategy>::operator()(ImprovedBranchBoundGraphBipartitioningSubproblem* sub_problem, size_t* upper_bound) {
	return Strategy();
}

template <class Strategy>
void ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Strategy>::print_name() {
	std::cout << "AutoStrategy<";
	Strategy::print_name();
	std::cout << ">";
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGAUTOSTRATEGY_H_ */
