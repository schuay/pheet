/*
 * ImprovedBranchBoundGraphBipartitioningSubproblem.h
 *
 *  Created on: Dec 1, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEM_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEM_H_

#include "pheet/pheet.h"
#include "../graph_helpers.h"
#include "pheet/primitives/Reducer/Max/MaxReducer.h"

#include "ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters.h"

#include <bitset>

namespace pheet {

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize = 64>
class ImprovedBranchBoundGraphBipartitioningSubproblem {
public:
	typedef std::bitset<MaxSize> Set;
	typedef ImprovedBranchBoundGraphBipartitioningSubproblemPerformanceCounters<Pheet> PerformanceCounters;
	typedef ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize> Self;
	typedef GraphBipartitioningSolution<MaxSize> Solution;
	typedef MaxReducer<Pheet, Solution> SolutionReducer;
	typedef LogicT<Pheet, Self> Logic;

	ImprovedBranchBoundGraphBipartitioningSubproblem(GraphVertex const* graph, size_t size, size_t k);
	ImprovedBranchBoundGraphBipartitioningSubproblem(Self const& other);
	~ImprovedBranchBoundGraphBipartitioningSubproblem();

	Self* split(PerformanceCounters& pc);
	bool is_solution();
	void update_solution(size_t* upper_bound, SolutionReducer& best, PerformanceCounters& pc);
	size_t get_lower_bound();
	size_t get_upper_bound();

	GraphVertex const* const graph;
	size_t const size;
	size_t const k;

	Set sets[3];
private:
	void update(uint8_t set, size_t pos);

	Logic logic;
};

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::ImprovedBranchBoundGraphBipartitioningSubproblem(GraphVertex const* graph, size_t size, size_t k)
: graph(graph), size(size), k(k), logic(this) {
	for(size_t i = 0; i < size; ++i) {
		sets[2].set(i);
	}
	if(k == size - k) {
		// If both groups have the same size we can already fill in the first vertex
		size_t nv = logic.get_next_vertex();
		update(0, nv);
	}
}

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::ImprovedBranchBoundGraphBipartitioningSubproblem(Self const& other)
: graph(other.graph), size(other.size), k(other.k), logic(this, other.logic) {
	for(size_t i = 0; i < 3; ++i) {
		sets[i] = other.sets[i];
	}
}

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::~ImprovedBranchBoundGraphBipartitioningSubproblem() {

}

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>* ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::split(PerformanceCounters& pc) {
	pc.num_allocated_subproblems.incr();
	pc.memory_allocation_time.start_timer();
	Self* other = new Self(*this);
	pc.memory_allocation_time.stop_timer();

	size_t nv = logic.get_next_vertex();
	update(0, nv);
	other->update(1, nv);

	return other;
}

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
void ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::update(uint8_t set, size_t pos) {
	assert((set & 1) == set);
	assert(pos < size);
	assert(!sets[set].test(pos));
	assert(sets[2].test(pos));

	sets[2].set(pos, false);
	sets[set].set(pos);

//	uint8_t other_set = set ^ 1;

	logic.update(set, pos);
}

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
bool ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::is_solution() {
	return (sets[0].count() == k) || (sets[1].count() == (size - k));
}

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
void ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::update_solution(size_t* upper_bound, MaxReducer<Pheet, GraphBipartitioningSolution<MaxSize> >& best, PerformanceCounters& pc) {
	if(sets[0].count() == k) {
		sets[1] |= sets[2];
		Set tmp = sets[2];
		sets[2].reset();
		logic.bulk_update(1, tmp);
	}
	else {
		assert(sets[1].count() == (size - k));
		sets[0] |= sets[2];
		Set tmp = sets[2];
		sets[2].reset();
		logic.bulk_update(0, tmp);
	}
	size_t cut = logic.get_cut();
	size_t old_ub = *upper_bound;
	if(cut < old_ub) {
		if(SIZET_CAS(upper_bound, old_ub, cut)) {
			pc.num_upper_bound_changes.incr();
			GraphBipartitioningSolution<MaxSize> my_best;
			my_best.weight = cut;
			my_best.sets[0] = sets[0];
			my_best.sets[1] = sets[1];
			best.add_value(my_best);
		}
	}
}

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
size_t ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::get_lower_bound() {
	return logic.get_lower_bound();
}

template <class Pheet, template <class P, class SP> class LogicT, size_t MaxSize>
size_t ImprovedBranchBoundGraphBipartitioningSubproblem<Pheet, LogicT, MaxSize>::get_upper_bound() {
	return logic.get_upper_bound();
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEM_H_ */
