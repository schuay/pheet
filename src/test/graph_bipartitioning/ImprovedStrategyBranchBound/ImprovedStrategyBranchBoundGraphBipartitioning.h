/*
 * ImprovedStrategyBranchBoundGraphBipartitioning.h
 *
 *  Created on: Dec 5, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_

#include "../graph_helpers.h"
#include "ImprovedStrategyBranchBoundGraphBipartitioningRootTask.h"

#include <iostream>

namespace pheet {

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE = 64>
class ImprovedStrategyBranchBoundGraphBipartitioning {
public:
	ImprovedStrategyBranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size);
	~ImprovedStrategyBranchBoundGraphBipartitioning();

	void partition();
	GraphBipartitioningSolution<MAX_SIZE> const& get_solution();

	void print_results();
	void print_headers();

	static void print_scheduler_name();

	static procs_t const max_cpus;
	static char const name[];
	static char const * const scheduler_name;

private:
	GraphVertex* data;
	size_t size;
	typename Scheduler::CPUHierarchy cpu_hierarchy;
	Scheduler scheduler;
	GraphBipartitioningSolution<MAX_SIZE> solution;
};

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
procs_t const ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::max_cpus = Scheduler::max_cpus;

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
char const ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::name[] = "ImprovedStrategyBranchBoundGraphBipartitioning";

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
char const * const ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::scheduler_name = Scheduler::name;

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::ImprovedStrategyBranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size)
: data(data), size(size), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::~ImprovedStrategyBranchBoundGraphBipartitioning() {

}

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
void ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::partition() {
	scheduler.template finish<ImprovedStrategyBranchBoundGraphBipartitioningRootTask<typename Scheduler::Task, Logic, SchedulingStrategy, MAX_SIZE> >(data, size, &solution);
}

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
GraphBipartitioningSolution<MAX_SIZE> const& ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::get_solution() {
	return solution;
}

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
void ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::print_results() {
	Logic::print_name();
	std::cout << "\t";
	SchedulingStrategy<Scheduler, void>::print_name();
	std::cout << "\t";
	scheduler.print_performance_counter_values();
}

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
void ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::print_headers() {
	std::cout << "logic\tstrategy\t";
	scheduler.print_performance_counter_headers();
}

template <class Scheduler, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
void ImprovedStrategyBranchBoundGraphBipartitioning<Scheduler, Logic, SchedulingStrategy, MAX_SIZE>::print_scheduler_name() {
	Scheduler::print_name();
}


}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_ */
