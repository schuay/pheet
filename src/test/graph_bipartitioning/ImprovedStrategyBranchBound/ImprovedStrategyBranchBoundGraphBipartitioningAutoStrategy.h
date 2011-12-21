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

namespace pheet {

template <class Strategy, class SubProblem>
class ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy();
	~ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy();

	Strategy operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
};

template <class Strategy, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Strategy, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy() {

}

template <class Strategy, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Strategy, SubProblem>::~ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy() {

}

template <class Strategy, class SubProblem>
inline Strategy ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Strategy, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	return Strategy();
}

template <class Strategy, class SubProblem>
void ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Strategy, SubProblem>::print_name() {
	std::cout << "AutoStrategy<";
	Strategy::print_name();
	std::cout << ">";
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGAUTOSTRATEGY_H_ */
