/*
 * ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters.h
 *
 *  Created on: Jan 12, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEMPERFORMANCECOUNTERS_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEMPERFORMANCECOUNTERS_H_

#include "../../../settings.h"

#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Time/TimePerformanceCounter.h"

namespace pheet {

template <class Scheduler>
class ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters {
public:
	ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters();
	ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters(ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>& other);
	~ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters();

	void print_headers();
	void print_values();

	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_upper_bound_changes> num_upper_bound_changes;
	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_allocated_subproblems> num_allocated_subproblems;

	TimePerformanceCounter<Scheduler, graph_bipartitioning_test_measure_memory_allocation_time> memory_allocation_time;
};


template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters()
{

}

template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters(ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>& other)
:num_upper_bound_changes(other.num_upper_bound_changes),
 num_allocated_subproblems(other.num_allocated_subproblems),
 memory_allocation_time(other.memory_allocation_time)
{

}

template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::~ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters() {

}

template <class Scheduler>
inline void ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::print_headers() {
	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_upper_bound_changes>::print_header("num_upper_bound_changes\t");
	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_allocated_subproblems>::print_header("num_allocated_subproblems\t");
	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_measure_memory_allocation_time>::print_header("memory_allocation_time\t");
}

template <class Scheduler>
inline void ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::print_values() {
	num_upper_bound_changes.print("%d\t");
	num_allocated_subproblems.print("%d\t");
	memory_allocation_time.print("%f\t");
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEMPERFORMANCECOUNTERS_H_ */
