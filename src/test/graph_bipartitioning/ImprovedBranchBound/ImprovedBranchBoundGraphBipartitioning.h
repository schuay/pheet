/*
 * ImprovedImprovedBranchBoundGraphBipartitioning.h
 *
 *  Created on: Dec 2, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONING_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONING_H_

#include "../graph_helpers.h"
#include "ImprovedBranchBoundGraphBipartitioningRootTask.h"

#include <iostream>

namespace pheet {

template <class Scheduler, class Logic, size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioning {
public:
	ImprovedBranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size);
	~ImprovedBranchBoundGraphBipartitioning();

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

template <class Scheduler, class Logic, size_t MAX_SIZE>
procs_t const ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::max_cpus = Scheduler::max_cpus;

template <class Scheduler, class Logic, size_t MAX_SIZE>
char const ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::name[] = "ImprovedBranchBoundGraphBipartitioning";

template <class Scheduler, class Logic, size_t MAX_SIZE>
char const * const ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::scheduler_name = Scheduler::name;

template <class Scheduler, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioning(procs_t cpus, GraphVertex* data, size_t size)
: data(data), size(size), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::~ImprovedBranchBoundGraphBipartitioning() {

}

template <class Scheduler, class Logic, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::partition() {
	scheduler.template finish<ImprovedBranchBoundGraphBipartitioningRootTask<typename Scheduler::Task, Logic, MAX_SIZE> >(data, size, &solution);
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
GraphBipartitioningSolution<MAX_SIZE> const& ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::get_solution() {
	return solution;
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::print_results() {
//	std::cout << LowerBound::name << "\t" << NextVertex::name << "\t";
	scheduler.print_performance_counter_values();
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::print_headers() {
//	std::cout << "lower_bound\tnext_vertex\t";
	scheduler.print_performance_counter_headers();
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioning<Scheduler, Logic, MAX_SIZE>::print_scheduler_name() {
	Scheduler::print_name();
}


}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONING_H_ */
