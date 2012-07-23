/*
 * PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy.h
 *
 *  Created on: Dec 21, 2011
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_
#define PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_

#include <pheet/pheet.h>
#include <pheet/sched/strategies/UserDefinedPriority/UserDefinedPriority.h>

namespace pheet {

template <class Pheet, class SubProblem>
class PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy : public Pheet::Environment::BaseStrategy {
public:
    typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet,SubProblem> Self;
    typedef typename Pheet::Environment::BaseStrategy BaseStrategy;
    
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(SubProblem* sub_problem, size_t* upper_bound);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(Self const& other);
    PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(Self && other);

	~PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy();

	inline bool prioritize(Self& other);

	//	UserDefinedPriority<Pheet> operator()(SubProblem* sub_problem, size_t* upper_bound);

	static void print_name();
private:
	size_t lower_bound;
};

template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(SubProblem* sub_problem, size_t* upper_bound)
:lower_bound(sub_problem->get_lower_bound()) {
	size_t ub = *upper_bound;
	size_t w = (ub)/(sub_problem->get_lower_bound() + 1);
	size_t w2 = std::min((size_t)28, sub_problem->size - sub_problem->sets[0].count() - sub_problem->sets[1].count());
	this->set_transitive_weight((size_t)1 << (std::min(w, w2) + 2));
}

template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(Self const& other)
:BaseStrategy(other),
 lower_bound(other.lower_bound)
{

}

template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy(Self&& other)
:BaseStrategy(other),
 lower_bound(other.lower_bound)
{

}

template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::~PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy() {

}

template <class Pheet, class SubProblem>
inline bool PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::prioritize(Self& other)
{
	return lower_bound < other.lower_bound;
}

/*
template <class Pheet, class SubProblem>
UserDefinedPriority<Pheet> PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	size_t lb = sub_problem->get_lower_bound();
	size_t ub = *upper_bound;

//	prio_t prio_pop = 1 + depth * bound_diff;
//	prio_t prio_steal = 1 + (sub_problem->size - depth) * bound_diff;

	return UserDefinedPriority<Pheet>(ub - lb, ub - lb);
	}*/

template <class Pheet, class SubProblem>
inline void PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy<Pheet, SubProblem>::print_name() {
	std::cout << "LowerBoundStrategy";
}

}

#endif /* PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGLOWERBOUNDSTRATEGY_H_ */
