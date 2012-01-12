/*
 * ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters.h
 *
 *  Created on: Jan 12, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_

#include "../../../settings.h"

#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "../ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters.h"

namespace pheet {

template <class Scheduler>
class ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters {
public:
	ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters();
	ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters(ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters& other);
	~ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters();

	void print_headers();
	void print_values();

	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_irrelevant_tasks> num_irrelevant_tasks;
	ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Scheduler> subproblem_pc;
};


template <class Scheduler>
inline ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters()
{

}

template <class Scheduler>
inline ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters(ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>& other)
:num_irrelevant_tasks(other.num_irrelevant_tasks),
 subproblem_pc(other.subproblem_pc)
{

}

template <class Scheduler>
inline ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::~ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters() {

}

template <class Scheduler>
inline void ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::print_headers() {
	BasicPerformanceCounter<Scheduler, graph_bipartitioning_test_count_irrelevant_tasks>::print_header("num_irrelevant_tasks\t");

	subproblem_pc.print_headers();
}

template <class Scheduler>
inline void ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<Scheduler>::print_values() {
	num_irrelevant_tasks.print("%d\t");

	subproblem_pc.print_values();
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_ */
