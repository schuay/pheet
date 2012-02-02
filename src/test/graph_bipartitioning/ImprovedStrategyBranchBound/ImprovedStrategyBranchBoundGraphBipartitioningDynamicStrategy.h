/*
 * ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy.h
 *
 *  Created on: Jan 13, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDYNAMICSTRATEGY_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDYNAMICSTRATEGY_H_

#include "../../../settings.h"

namespace pheet {

template <class Scheduler, template <class EScheduler, class ESubProblem> class Strategy, class SubProblem>
class ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy();
	~ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy();

	Strategy<Scheduler, SubProblem> operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
};

template <class Scheduler, template <class EScheduler, class ESubProblem> class Strategy, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy<Scheduler, Strategy, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy() {

}

template <class Scheduler, template <class EScheduler, class ESubProblem> class Strategy, class SubProblem>
inline ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy<Scheduler, Strategy, SubProblem>::~ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy() {

}

template <class Scheduler, template <class EScheduler, class ESubProblem> class Strategy, class SubProblem>
inline Strategy<Scheduler, SubProblem> ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy<Scheduler, Strategy, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	return Strategy<Scheduler, SubProblem>(sub_problem, upper_bound);
}

template <class Scheduler, template <class EScheduler, class ESubProblem> class Strategy, class SubProblem>
void ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy<Scheduler, Strategy, SubProblem>::print_name() {
	std::cout << "DynamicStrategy<";
	Strategy<Scheduler, SubProblem>::print_name();
	std::cout << ">";
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGDYNAMICSTRATEGY_H_ */
