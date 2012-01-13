/*
 * ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy.h
 *
 *  Created on: Jan 13, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDFIFOSTRATEGY_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDFIFOSTRATEGY_H_

#include "../../../settings.h"
#include "../../../sched/strategies/BaseStrategy.h"

namespace pheet {

template <class Scheduler, class SubProblem>
class ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy : public BaseStrategy<Scheduler> {
public:
	typedef ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem> Self;
	ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(SubProblem* sub_problem, size_t* upper_bound);
	ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(Self const& other);
	ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(Self const&& other);
	~ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy();

	virtual prio_t get_pop_priority(size_t task_id);
	virtual prio_t get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc);
	virtual BaseStrategy<Scheduler>* clone();

	static void print_name();

private:
	prio_t pop_prio;
};

template <class Scheduler, class SubProblem>
ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(SubProblem* sub_problem, size_t* upper_bound)
:pop_prio(std::numeric_limits< prio_t >::max() - sub_problem->get_upper_bound())
{

}

template <class Scheduler, class SubProblem>
ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(Self const& other)
:pop_prio(other.pop_prio)
{

}

template <class Scheduler, class SubProblem>
ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem>::ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(Self const&& other)
:pop_prio(other.pop_prio)
{

}

template <class Scheduler, class SubProblem>
ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem>::~ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy() {

}
template <class Scheduler, class SubProblem>
inline prio_t ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem>::get_pop_priority(size_t task_id) {
	return pop_prio;
}

template <class Scheduler, class SubProblem>
inline prio_t ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem>::get_steal_priority(size_t task_id, typename Scheduler::StealerDescriptor& desc) {
	return std::numeric_limits< prio_t >::max() - task_id;
}

template <class Scheduler, class SubProblem>
inline BaseStrategy<Scheduler>* ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem>::clone() {
	return new Self(*this);
}

template <class Scheduler, class SubProblem>
inline void ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Scheduler, SubProblem>::print_name() {
	std::cout << "UpperBoundFifoStrategy";
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDFIFOSTRATEGY_H_ */
