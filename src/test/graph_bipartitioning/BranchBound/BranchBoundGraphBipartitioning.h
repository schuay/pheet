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

template <class Scheduler, class LowerBound, class NextVertex>
class BranchBoundGraphBipartitioning {
public:
	BranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size);
	~BranchBoundGraphBipartitioning();

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

template <class Scheduler, class LowerBound, class NextVertex>
procs_t const BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::max_cpus = Scheduler::max_cpus;

template <class Scheduler, class LowerBound, class NextVertex>
char const BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::name[] = "BranchBoundGraphBipartitioning";

template <class Scheduler, class LowerBound, class NextVertex>
char const * const BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::scheduler_name = Scheduler::name;

template <class Scheduler, class LowerBound, class NextVertex>
BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::BranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size)
: data(data), size(size), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler, class LowerBound, class NextVertex>
BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::~BranchBoundGraphBipartitioning() {

}

template <class Scheduler, class LowerBound, class NextVertex>
void BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::partition() {
	scheduler.template finish<BranchBoundGraphBipartitioningRootTask<typename Scheduler::Task, LowerBound, NextVertex> >(data, size, &solution);
}

template <class Scheduler, class LowerBound, class NextVertex>
GraphBipartitioningSolution const& BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::get_solution() {
	return solution;
}

template <class Scheduler, class LowerBound, class NextVertex>
void BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::print_results() {
	std::cout << LowerBound::name << "\t" << NextVertex::name << "\t";
	scheduler.print_performance_counter_values();
}

template <class Scheduler, class LowerBound, class NextVertex>
void BranchBoundGraphBipartitioning<Scheduler, LowerBound, NextVertex>::print_headers() {
	std::cout << "lower_bound\tnext_vertex\t";
	scheduler.print_performance_counter_headers();
}


}

#endif /* BRANCHBOUNDGRAPHBIPARTITIONING_H_ */
