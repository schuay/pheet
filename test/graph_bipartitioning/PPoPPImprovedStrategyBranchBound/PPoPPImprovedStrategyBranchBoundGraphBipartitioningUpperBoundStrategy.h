/*
 * ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy.h
 *
 *  Created on: Jan 13, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDSTRATEGY_H_
#define PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDSTRATEGY_H_

#include <pheet/pheet.h>
#include <pheet/sched/strategies/UserDefinedPriority/UserDefinedPriority.h>

namespace pheet {

template <class Pheet, class SubProblem>
  class PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy : public Pheet::Environment::BaseStrategy {
public:
    typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy<Pheet,SubProblem> Self;
    typedef typename Pheet::Environment::BaseStrategy BaseStrategy;

    PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy(SubProblem* sub_problem, size_t* upper_bound);
//    PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy(Self const& other);
//    PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy(Self && other);


	~PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy();



	//	UserDefinedPriority<Pheet> operator()(SubProblem* sub_problem, size_t* upper_bound);

	inline bool prioritize(Self& other);
/*
	inline bool forbid_call_conversion() const {
		return !last;
	}*/

	inline void rebase() {
		this->reset();
	}

	static void print_name();
private:
    size_t upper_bound;
    size_t uncertainty;
};

template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy(SubProblem* sub_problem, size_t* upper_bound)
:upper_bound(sub_problem->get_estimate()),
 uncertainty(sub_problem->get_upper_bound() - sub_problem->get_lower_bound()) {
//	size_t depth = sub_problem->sets[0].count() - sub_problem->sets[1].count();
	size_t ub = *upper_bound;
	size_t w = (ub)/(sub_problem->get_lower_bound() + 1);
	size_t w2 = sub_problem->size - sub_problem->sets[0].count() - sub_problem->sets[1].count();
	this->set_transitive_weight(1 << (std::min(w, w2) + 2));
}
/*
template <class Pheet, class SubProblem>
  inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy(Self const& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
{

}

 template <class Pheet, class SubProblem>
   inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy<Pheet, SubProblem>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy(Self&& other):BaseStrategy(other),sub_problem(other.sub_problem),upper_bound(other.upper_bound)
   {

   }
*/

template <class Pheet, class SubProblem>
inline PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy<Pheet, SubProblem>::~PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy() {

}

template <class Pheet, class SubProblem>
inline bool PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy<Pheet, SubProblem>::prioritize(Self& other)
{
	if(this->get_place() == other.get_place() && this->get_place() == Pheet::get_place()) {
		return upper_bound < other.upper_bound;
	}
	return uncertainty > other.uncertainty;
}

/*
template <class Pheet, class SubProblem>
UserDefinedPriority<Pheet> PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy<Pheet, SubProblem>::operator()(SubProblem* sub_problem, size_t* upper_bound) {
	size_t lb = sub_problem->get_lower_bound();
	size_t ub = sub_problem->get_upper_bound();

	pheet_assert(ub >= lb);

	// pop task with lowest upper bound (often depth first, but not in extreme cases
	// (upper bound is "best guaranteed upper bound for this subtree" doesn't mean
	// that all branches will be below it!)
	prio_t prio_pop = std::numeric_limits< prio_t >::max() - ub;

	// steal task with highest range of uncertainty
	prio_t prio_steal = ub - lb;

	return UserDefinedPriority<Pheet>(prio_pop, prio_steal);
}
*/


template <class Pheet, class SubProblem>
inline void PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy<Pheet, SubProblem>::print_name() {
	std::cout << "UpperBoundStrategy";
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGUPPERBOUNDSTRATEGY_H_ */
