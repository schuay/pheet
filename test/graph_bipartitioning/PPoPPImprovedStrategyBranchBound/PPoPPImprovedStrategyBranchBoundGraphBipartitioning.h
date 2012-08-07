/*
 * ImprovedStrategyBranchBoundGraphBipartitioning.h
 *
 *  Created on: Dec 5, 2011
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_
#define PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_

#include "../graph_helpers.h"
#include "../ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningSubproblem.h"
#include "../ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic.h"
#include "PPoPPImprovedStrategyBranchBoundGraphBipartitioningEstimateStrategy.h"
#include "../ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters.h"
#include "PPoPPImprovedStrategyBranchBoundGraphBipartitioningTask.h"

#include <iostream>

namespace pheet {

template <class Pheet, template <class P, class SP> class Logic, template <class P, class SubProblem> class SchedulingStrategy, size_t MaxSize = 64>
class PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl {
public:
	typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, MaxSize> Self;
	typedef GraphBipartitioningSolution<MaxSize> Solution;
	typedef MaxReducer<Pheet, Solution> SolutionReducer;
	typedef ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<Pheet> PerformanceCounters;
	typedef ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, Logic, MaxSize> SubProblem;
	typedef PPoPPImprovedStrategyBranchBoundGraphBipartitioningTask<Pheet, Logic, SchedulingStrategy, MaxSize> BBTask;

	template <template <class P, class SP> class NewLogic>
		using WithLogic = PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, NewLogic, SchedulingStrategy, MaxSize>;

	template <template <class P, class SP> class NewStrat>
		using WithSchedulingStrategy = PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, NewStrat, MaxSize>;

	template <size_t ms>
		using WithMaxSize = PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, ms>;

	template <class P>
		using BT = PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<P, Logic, SchedulingStrategy, MaxSize>;


	PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl(GraphVertex* data, size_t size, Solution& solution, PerformanceCounters& pc);
	~PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl();

	void operator()();

	static void print_headers();
	static void print_configuration();

	static char const name[];

private:
	GraphVertex* data;
	size_t size;
	Solution& solution;
	PerformanceCounters pc;
};

template <class Pheet, template <class P, class SubProblem> class Logic, template <class P, class SubProblem> class SchedulingStrategy, size_t MaxSize>
char const PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, MaxSize>::name[] = "PPoPPImprovedStrategyBranchBoundGraphBipartitioning";

template <class Pheet, template <class P, class SubProblem> class Logic, template <class P, class SubProblem> class SchedulingStrategy, size_t MaxSize>
PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, MaxSize>::PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl(GraphVertex* data, size_t size, Solution& solution, PerformanceCounters& pc)
: data(data), size(size), solution(solution), pc(pc) {

}

template <class Pheet, template <class P, class SubProblem> class Logic, template <class P, class SubProblem> class SchedulingStrategy, size_t MaxSize>
PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, MaxSize>::~PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl() {

}

template <class Pheet, template <class P, class SubProblem> class Logic, template <class P, class SubProblem> class SchedulingStrategy, size_t MaxSize>
void PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, MaxSize>::operator()() {
	SolutionReducer best;

	size_t ub = std::numeric_limits< size_t >::max();

	size_t k = size >> 1;
	SubProblem* prob = new SubProblem(data, size, k);
	Pheet::template
		finish<BBTask>(prob, &ub, best, pc);

	solution = best.get_max();
	pheet_assert(solution.weight == ub);
	pheet_assert(ub != std::numeric_limits< size_t >::max());
	pheet_assert(solution.weight != std::numeric_limits< size_t >::max());
	pheet_assert(solution.sets[0].count() == k);
	pheet_assert(solution.sets[1].count() == size - k);
}
/*
template <class Pheet, template <class P, class SubProblem> class Logic, template <class P, class SubProblem> class SchedulingStrategy, size_t MaxSize>
void PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, MaxSize>::partition(SubProblem* sub_problem, size_t* upper_bound, SolutionReducer& best, PerformanceCounters& pc) {
	if(sub_problem->get_lower_bound() >= *upper_bound) {
		delete sub_problem;
		pc.num_irrelevant_tasks.incr();
		return;
	}
	SchedulingStrategy<Pheet, SubProblem> strategy;

	SubProblem* sub_problem2 =
			sub_problem->split(pc.subproblem_pc);

	if(sub_problem->is_solution()) {
		sub_problem->update_solution(upper_bound, best, pc.subproblem_pc);
		delete sub_problem;
	}
	else if(sub_problem->get_lower_bound() < *upper_bound) {
		Pheet::spawn_prio(strategy(sub_problem, upper_bound),
				Self::partition,
				sub_problem, upper_bound, std::move(SolutionReducer(best)), std::move(PerformanceCounters(pc)));
	}
	else {
		delete sub_problem;
	}

	if(sub_problem2->is_solution()) {
		sub_problem2->update_solution(upper_bound, best, pc.subproblem_pc);
		delete sub_problem2;
	}
	else if(sub_problem2->get_lower_bound() < *upper_bound) {
		Pheet::spawn_prio(strategy(sub_problem2, upper_bound),
				partition,
				sub_problem2, upper_bound, best, pc);
	}
	else {
		delete sub_problem2;
	}
}*/

template <class Pheet, template <class P, class SubProblem> class Logic, template <class P, class SubProblem> class SchedulingStrategy, size_t MaxSize>
void PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, MaxSize>::print_configuration() {
	Logic<Pheet, SubProblem>::print_name();
	std::cout << "\t";
	SchedulingStrategy<Pheet, SubProblem>::print_name();
	std::cout << "\t";
}

template <class Pheet, template <class P, class SubProblem> class Logic, template <class P, class SubProblem> class SchedulingStrategy, size_t MaxSize>
void PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, Logic, SchedulingStrategy, MaxSize>::print_headers() {
	std::cout << "logic\tstrategy\t";
}

template <class Pheet = Pheet>
using PPoPPImprovedStrategyBranchBoundGraphBipartitioning = PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic, PPoPPImprovedStrategyBranchBoundGraphBipartitioningEstimateStrategy, 64>;
/*
template <class Pheet = Pheet>
using OldPPoPPImprovedStrategyBranchBoundGraphBipartitioning = PPoPPImprovedStrategyBranchBoundGraphBipartitioningImpl<Pheet, ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic, PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy, 64>;
*/
}

#endif /* PPOPPIMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_ */
