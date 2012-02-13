/*
 * ImprovedBranchBoundGraphBipartitioningPerformanceCounters.h
 *
 *  Created on: Jan 12, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_

#include "../../../settings.h"

#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters.h"

namespace pheet {

template <class Scheduler>
class ImprovedBranchBoundGraphBipartitioningPerformanceCounters {
public:
	ImprovedBranchBoundGraphBipartitioningPerformanceCounters();
	ImprovedBranchBoundGraphBipartitioningPerformanceCounters(ImprovedBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>& other);
	~ImprovedBranchBoundGraphBipartitioningPerformanceCounters();

	void print_headers();
	void print_values();

	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_irrelevant_tasks> num_irrelevant_tasks;
	ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler> subproblem_pc;
};

template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::ImprovedBranchBoundGraphBipartitioningPerformanceCounters()
{

}

template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::ImprovedBranchBoundGraphBipartitioningPerformanceCounters(ImprovedBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>& other)
:num_irrelevant_tasks(other.num_irrelevant_tasks),
 subproblem_pc(other.subproblem_pc)
{

}

template <class Scheduler>
inline ImprovedBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::~ImprovedBranchBoundGraphBipartitioningPerformanceCounters() {

}

template <class Scheduler>
inline void ImprovedBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::print_headers() {
	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_irrelevant_tasks>::print_header("num_irrelevant_tasks\t");

	subproblem_pc.print_headers();
}

template <class Scheduler>
inline void ImprovedBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::print_values() {
	num_irrelevant_tasks.print("%d\t");

	subproblem_pc.print_values();
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_ */
