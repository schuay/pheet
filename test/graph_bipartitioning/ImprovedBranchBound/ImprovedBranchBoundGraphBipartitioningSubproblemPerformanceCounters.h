/*
 * ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters.h
 *
 *  Created on: Jan 12, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEMPERFORMANCECOUNTERS_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEMPERFORMANCECOUNTERS_H_

#include "pheet/pheet.h"

#include "pheet/primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "pheet/primitives/PerformanceCounter/Time/TimePerformanceCounter.h"
#include "pheet/primitives/Reducer/List/ListReducer.h"
#include <vector>


namespace pheet {

template <typename T>
class Event
{
	struct timeval start;
	T value;
public:
	Event(struct timeval start, T value):start(start),value(value)
	{}
	void print(struct timeval expstart)
	{
		double time = (start.tv_sec - expstart.tv_sec) + 1.0e-6 * start.tv_usec - 1.0e-6 * expstart.tv_usec;

		std::cout << time << ": " << value << std::endl;
	}

};


template <class Pheet>
class ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters {
public:
	ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters();
	ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters(ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Pheet>& other);
	~ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters();

	static void print_headers();
	void print_values();

	BasicPerformanceCounter<Pheet, graph_bipartitioning_test_count_upper_bound_changes> num_upper_bound_changes;
	BasicPerformanceCounter<Pheet, graph_bipartitioning_test_count_allocated_subproblems> num_allocated_subproblems;

	TimePerformanceCounter<Pheet, graph_bipartitioning_test_measure_memory_allocation_time> memory_allocation_time;
	ListReducer<Pheet, std::vector<Event<size_t> >, Event<size_t> > events;

};


template <class Pheet>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Pheet>::ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters()
{

}

template <class Pheet>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Pheet>::ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters(ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Pheet>& other)
:num_upper_bound_changes(other.num_upper_bound_changes),
 num_allocated_subproblems(other.num_allocated_subproblems),
 memory_allocation_time(other.memory_allocation_time),
 events(other.events)
{

}

template <class Pheet>
inline ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Pheet>::~ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters() {

}

template <class Pheet>
inline void ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Pheet>::print_headers() {
	BasicPerformanceCounter<Pheet, graph_bipartitioning_test_count_upper_bound_changes>::print_header("num_upper_bound_changes\t");
	BasicPerformanceCounter<Pheet, graph_bipartitioning_test_count_allocated_subproblems>::print_header("num_allocated_subproblems\t");
	BasicPerformanceCounter<Pheet, graph_bipartitioning_test_measure_memory_allocation_time>::print_header("memory_allocation_time\t");
}

template <class Pheet>
inline void ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Pheet>::print_values(struct timeval expstart) {
	num_upper_bound_changes.print("%d\t");
	num_allocated_subproblems.print("%d\t");
	memory_allocation_time.print("%f\t");

	std::vector<Event<size_t> > eventslist = events.get_list();
	for(size_t i=0;i<eventslist.size();i++)
		eventslist[i].print();

}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEMPERFORMANCECOUNTERS_H_ */
