/*
 * ImprovedBranchBoundGraphBipartitioningDeltaLogic.h
 *
 *  Created on: Dec 21, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTALOGIC_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTALOGIC_H_

#include "../../../settings.h"
#include "../graph_helpers.h"

#include <bitset>
#include <string.h>
#include <algorithm>

namespace pheet {

template <class Scheduler, size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioningDeltaLogic {
public:
	typedef ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE> Self;
	typedef ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Self, MAX_SIZE> Subproblem;

	ImprovedBranchBoundGraphBipartitioningDeltaLogic(Subproblem* sub_problem);
	ImprovedBranchBoundGraphBipartitioningDeltaLogic(Subproblem* sub_problem, Self const& other);
	~ImprovedBranchBoundGraphBipartitioningDeltaLogic();

	void init_root();
	size_t get_next_vertex();
	size_t get_cut();
	size_t get_lower_bound();
	void update(uint8_t set, size_t pos);
	void bulk_update(uint8_t set, std::bitset<MAX_SIZE> positions);

	static void print_name();
private:
	Subproblem* sub_problem;

	size_t cut;
	size_t lb;

	size_t* weights[2];
};

template <class Scheduler, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningDeltaLogic(Subproblem* sub_problem)
: sub_problem(sub_problem), cut(0), lb(0) {
	weights[0] = new size_t[sub_problem->size];
	weights[1] = new size_t[sub_problem->size];
	memset(weights[0], 0, sizeof(size_t)*sub_problem->size);
	memset(weights[1], 0, sizeof(size_t)*sub_problem->size);
}

template <class Scheduler, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningDeltaLogic(Subproblem* sub_problem, Self const& other)
: sub_problem(sub_problem), cut(other.cut), lb(other.lb) {
	weights[0] = new size_t[sub_problem->size];
	weights[1] = new size_t[sub_problem->size];
	memcpy(weights[0], other.weights[0], sizeof(size_t)*sub_problem->size);
	memcpy(weights[1], other.weights[1], sizeof(size_t)*sub_problem->size);
}

template <class Scheduler, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::~ImprovedBranchBoundGraphBipartitioningDeltaLogic() {
	delete[] weights[0];
	delete[] weights[1];
}

template <class Scheduler, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::init_root() {

}

template <class Scheduler, size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::get_next_vertex() {
	return sub_problem->sets[2]._Find_first();
}

template <class Scheduler, size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::get_cut() {
	return cut;
}

template <class Scheduler, size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::get_lower_bound() {
	return get_cut() + lb;
}

template <class Scheduler, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::update(uint8_t set, size_t pos) {
	cut += weights[set ^ 1][pos];

	for(size_t i = 0; i < sub_problem->graph[pos].num_edges; ++i) {
		weights[set][sub_problem->graph[pos].edges[i].target] += sub_problem->graph[pos].edges[i].weight;
	}

	ptrdiff_t* delta = new ptrdiff_t[sub_problem->sets[2].count()];
	size_t di = 0;
	lb = 0;
	size_t current_bit = sub_problem->sets[2]._Find_first();
	while(current_bit != sub_problem->sets[2].size()) {
		delta[di++] = (ptrdiff_t)weights[1][current_bit] - (ptrdiff_t)weights[0][current_bit];
		lb += std::min(weights[0][current_bit], weights[1][current_bit]);
		current_bit = sub_problem->sets[2]._Find_next(current_bit);
	}
	std::sort(delta, delta + di);

	assert(di == sub_problem->sets[2].count());
	size_t pivot = (/*sub_problem->size - */sub_problem->k) - sub_problem->sets[0].count();
	if(pivot < di && delta[pivot] < 0) {
		do {
			lb += -delta[pivot];
			++pivot;
		} while(pivot < di && delta[pivot] < 0);
	}
	else if(pivot > 0 && delta[pivot - 1] > 0) {
		do {
			--pivot;
			lb += delta[pivot];
		} while(pivot > 0 && delta[pivot - 1] > 0);
	}

	delete[] delta;
}

template <class Scheduler, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::bulk_update(uint8_t set, std::bitset<MAX_SIZE> positions) {
	size_t current_bit = positions._Find_first();
	while(current_bit != positions.size()) {
		update(set, current_bit);
		current_bit = positions._Find_next(current_bit);
	}
}

template <class Scheduler, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningDeltaLogic<Scheduler, MAX_SIZE>::print_name() {
	std::cout << "DeltaLogic";
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTALOGIC_H_ */
