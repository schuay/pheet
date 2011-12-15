/*
 * ImprovedBranchBoundGraphBipartitioningSubproblem.h
 *
 *  Created on: Dec 1, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEM_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEM_H_

#include "../../../settings.h"
#include "../graph_helpers.h"
#include "../../../primitives/Reducer/Max/MaxReducer.h"

#include <bitset>

namespace pheet {

template <class Scheduler, class Logic, size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioningSubproblem {
public:
	typedef std::bitset<MAX_SIZE> Set;
	ImprovedBranchBoundGraphBipartitioningSubproblem(GraphVertex const* graph, size_t size, size_t k);
	ImprovedBranchBoundGraphBipartitioningSubproblem(ImprovedBranchBoundGraphBipartitioningSubproblem const& other);
	~ImprovedBranchBoundGraphBipartitioningSubproblem();

	ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>* split();
	bool is_solution();
	void update_solution(size_t* upper_bound, MaxReducer<Scheduler, GraphBipartitioningSolution<MAX_SIZE> >& best);
	size_t get_lower_bound();

	GraphVertex const* const graph;
	size_t const size;
	size_t const k;

	Set sets[3];
private:
	void update(uint8_t set, size_t pos);

	Logic logic;
};

template <class Scheduler, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningSubproblem(GraphVertex const* graph, size_t size, size_t k)
: graph(graph), size(size), k(k), logic(this) {
	for(size_t i = 0; i < size; ++i) {
		sets[2].set(i);
	}
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningSubproblem(ImprovedBranchBoundGraphBipartitioningSubproblem const& other)
: graph(other.graph), size(other.size), k(other.k), logic(this, other.logic) {
	for(size_t i = 0; i < 3; ++i) {
		sets[i] = other.sets[i];
	}
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>::~ImprovedBranchBoundGraphBipartitioningSubproblem() {

}

template <class Scheduler, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>* ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>::split() {
	ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>* other = new ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>(*this);

	size_t nv = logic.get_next_vertex();
	update(0, nv);
	other->update(1, nv);

	return other;
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>::update(uint8_t set, size_t pos) {
	assert((set & 1) == set);
	assert(pos < size);
	assert(!sets[set].test(pos));
	assert(sets[2].test(pos));

	sets[2].set(pos, false);
	sets[set].set(pos);

//	uint8_t other_set = set ^ 1;

	logic.update(set, pos);
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
bool ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>::is_solution() {
	return (sets[0].count() == k) || (sets[1].count() == (size - k));
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>::update_solution(size_t* upper_bound, MaxReducer<Scheduler, GraphBipartitioningSolution<MAX_SIZE> >& best) {
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
			if(cut == 49236) {

			}
			GraphBipartitioningSolution<MAX_SIZE> my_best;
			my_best.weight = cut;
			my_best.sets[0] = sets[0];
			my_best.sets[1] = sets[1];
			best.add_value(my_best);
		}
	}
}

template <class Scheduler, class Logic, size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Logic, MAX_SIZE>::get_lower_bound() {
	return logic.get_lower_bound();
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGSUBPROBLEM_H_ */
