/*
 * BranchBoundGraphBipartitioning.h
 *
 *  Created on: 08.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef BRANCHBOUNDGRAPHBIPARTITIONING_H_
#define BRANCHBOUNDGRAPHBIPARTITIONING_H_

#include "../graph_helpers.h"
#include "BranchBoundGraphBipartitioningRootTask.h"

#include <iostream>

/*
 *
 */
namespace pheet {

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE = 64>
class BranchBoundGraphBipartitioning {
public:
	BranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size);
	~BranchBoundGraphBipartitioning();

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

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
procs_t const BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::max_cpus = Scheduler::max_cpus;

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
char const BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::name[] = "BranchBoundGraphBipartitioning";

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
char const * const BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::scheduler_name = Scheduler::name;

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::BranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size)
: data(data), size(size), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::~BranchBoundGraphBipartitioning() {

}

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
void BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::partition() {
	scheduler.template finish<BranchBoundGraphBipartitioningRootTask<typename Scheduler::Task, LowerBound, NextVertex, MAX_SIZE> >(data, size, &solution);
}

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
GraphBipartitioningSolution<MAX_SIZE> const& BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::get_solution() {
	return solution;
}

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
void BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::print_results() {
	std::cout << LowerBound::name << "\t" << NextVertex::name << "\t";
	scheduler.print_performance_counter_values();
}

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
void BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::print_headers() {
	std::cout << "lower_bound\tnext_vertex\t";
	scheduler.print_performance_counter_headers();
}

template <class Scheduler, class LowerBound, class NextVertex, size_t MAX_SIZE>
void BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex, MAX_SIZE>::print_scheduler_name() {
	Scheduler::print_name();
}

}

#endif /* BRANCHBOUNDGRAPHBIPARTITIONING_H_ */
