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

template <class Scheduler, template <class EScheduler> class Strategy, class SubProblem>
class ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy();
	~ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy();

	Strategy<Scheduler> operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
};

template <class Scheduler, template <class EScheduler> class Strategy, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, Strategy, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy() {

}

template <class Scheduler, template <class EScheduler> class Strategy, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, Strategy, SubProblem>::~ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy() {

}

template <class Scheduler, template <class EScheduler> class Strategy, class SubProblem>
inline Strategy<Scheduler> ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, Strategy, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	return Strategy<Scheduler>();
}

template <class Scheduler, template <class EScheduler> class Strategy, class SubProblem>
void ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, Strategy, SubProblem>::print_name() {
	std::cout << "AutoStrategy<";
	Strategy<Scheduler>::print_name();
	std::cout << ">";
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGAUTOSTRATEGY_H_ */
