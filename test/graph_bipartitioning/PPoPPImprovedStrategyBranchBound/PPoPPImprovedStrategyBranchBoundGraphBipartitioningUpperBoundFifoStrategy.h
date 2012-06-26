/*
 * PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy.h
 *
 *  Created on: Jan 13, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDFIFOSTRATEGY_H_
#define PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDFIFOSTRATEGY_H_

#include <pheet/pheet.h>
#include <pheet/sched/strategies/BaseStrategy.h>

namespace pheet {

template <class Pheet, class SubProblem>
  class PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy : public Pheet::Environment::BaseStrategy {
public:
	typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem> Self;
	typedef typename Pheet::Environment::BaseStrategy BaseStrategy;

	PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(SubProblem* sub_problem, size_t* upper_bound);
	PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(Self const& other);
	PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(Self&& other);
	~PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy();

	inline bool prioritize(Self& other);
	//	virtual prio_t get_pop_priority(size_t task_id);
	//virtual prio_t get_steal_priority(size_t task_id, typename Pheet::Environment::StealerDescriptor& desc);
	//virtual BaseStrategy<Pheet>* clone();

	static void print_name();

private:
	prio_t pop_prio;
};

template <class Pheet, class SubProblem>
PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(SubProblem* sub_problem, size_t* upper_bound)
:pop_prio(std::numeric_limits< prio_t >::max() - sub_problem->get_upper_bound())
{

}

template <class Pheet, class SubProblem>
PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(Self const& other)
  :BaseStrategy(other),pop_prio(other.pop_prio)
{

}

template <class Pheet, class SubProblem>
PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy(Self && other)
  :BaseStrategy(other),pop_prio(other.pop_prio)
{

}

template <class Pheet, class SubProblem>
PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::~PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy() {

}

 template <class Pheet, class SubProblem>
   inline bool PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::prioritize(Self& other) {
   return BaseStrategy::prioritize(other);
 }

/*

template <class Pheet, class SubProblem>
inline prio_t PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::get_pop_priority(size_t task_id) {
	return pop_prio;
}

template <class Pheet, class SubProblem>
inline prio_t PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::get_steal_priority(size_t task_id, typename Pheet::Environment::StealerDescriptor& desc) {
	return std::numeric_limits< prio_t >::max() - task_id;
}

template <class Pheet, class SubProblem>
inline BaseStrategy<Pheet>* PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::clone() {
	return new Self(*this);
}
*/

template <class Pheet, class SubProblem>
inline void PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy<Pheet, SubProblem>::print_name() {
	std::cout << "UpperBoundFifoStrategy";
}

}

#endif /* PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDFIFOSTRATEGY_H_ */
