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

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE = 64>
class StrategyBranchBoundGraphBipartitioning {
public:
	StrategyBranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size);
	~StrategyBranchBoundGraphBipartitioning();

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

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
procs_t const StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::max_cpus = Scheduler::max_cpus;

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
char const StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::name[] = "StrategyBranchBoundGraphBipartitioning";

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
char const * const StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::scheduler_name = Scheduler::name;

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::StrategyBranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size)
: data(data), size(size), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::~StrategyBranchBoundGraphBipartitioning() {

}

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
void StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::partition() {
	scheduler.template finish<StrategyBranchBoundGraphBipartitioningRootTask<typename Scheduler::Task, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE> >(data, size, &solution);
}

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
GraphBipartitioningSolution<MAX_SIZE> const& StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::get_solution() {
	return solution;
}

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
void StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::print_results() {
	std::cout << LowerBound::name << "\t" << NextVertex::name << "\t";
	SchedulingStrategy<Scheduler>::print_name();
	std::cout << "\t";
	scheduler.print_performance_counter_values();
}

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
void StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::print_headers() {
	std::cout << "lower_bound\tnext_vertex\tstrategy\t";
	scheduler.print_performance_counter_headers();
}

template <class Scheduler, class LowerBound, class NextVertex, template <class EScheduler> class SchedulingStrategy, size_t MAX_SIZE>
void StrategyBranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::print_scheduler_name() {
	Scheduler::print_name();
}

}

#endif /* STRATEGYBRANCHBOUNDGRAPHBIPARTITIONING_H_ */
