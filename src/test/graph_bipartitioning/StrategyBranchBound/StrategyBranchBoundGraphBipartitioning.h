/*
 * StrategyBranchBoundGraphBipartitioning.h
 *
 *  Created on: 08.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef STRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_
#define STRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_

#include "../graph_helpers.h"
#include "StrategyBranchBoundGraphBipartitioningRootTask.h"

#include <iostream>

/*
 *
 */
namespace pheet {

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
class StrategyBranchBoundGraphBipartitioning {
public:
	StrategyBranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size);
	~StrategyBranchBoundGraphBipartitioning();

	void partition();
	GraphBipartitioningSolution const& get_solution();

	void print_results();
	void print_headers();

	static procs_t const max_cpus;
	static char const name[];
	static char const * const scheduler_name;

private:
	GraphVertex* data;
	size_t size;
	typename Scheduler::CPUHierarchy cpu_hierarchy;
	Scheduler scheduler;
	GraphBipartitioningSolution solution;
};

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
procs_t const StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::max_cpus = Scheduler::max_cpus;

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
char const StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::name[] = "StrategyBranchBoundGraphBipartitioning";

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
char const * const StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::scheduler_name = Scheduler::name;

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::StrategyBranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size)
: data(data), size(size), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::~StrategyBranchBoundGraphBipartitioning() {

}

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
void StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::partition() {
	scheduler.template finish<StrategyBranchBoundGraphBipartitioningRootTask<typename Scheduler::Task, LowerBound, NextVertex, SchedulingStrategy> >(data, size, &solution);
}

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
GraphBipartitioningSolution const& StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::get_solution() {
	return solution;
}

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
void StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::print_results() {
	std::cout << LowerBound::name << "\t" << NextVertex::name << "\t";
	SchedulingStrategy::print_name();
	std::cout << "\t";
	scheduler.print_performance_counter_values();
}

template <class Scheduler, class LowerBound, class NextVertex, class SchedulingStrategy>
void StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy>::print_headers() {
	std::cout << "lower_bound\tnext_vertex\tstrategy\t";
	scheduler.print_performance_counter_headers();
}


}

#endif /* STRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_ */
