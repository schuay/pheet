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
};


template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters()
{

}

template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters(ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>& other)
:num_upper_bound_changes(other.num_upper_bound_changes)
{

}

template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::~ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters() {

}

template <class Scheduler>
inline void ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::print_headers() {
	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_upper_bound_changes>::print_header("num_upper_bound_changes\t");
}

template <class Scheduler>
inline void ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler>::print_values() {
	num_upper_bound_changes.print("%d\t");
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEMPERFORMANCECOUNTERS_H_ */
