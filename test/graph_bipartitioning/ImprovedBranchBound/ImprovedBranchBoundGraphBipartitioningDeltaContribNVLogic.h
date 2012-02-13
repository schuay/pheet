/*
 * ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic.h
 *
 *  Created on: Jan 13, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTACONTRIBNVLOGIC_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTACONTRIBNVLOGIC_H_

#include "../../../settings.h"
#include "../graph_helpers.h"

#include <bitset>
#include <string.h>
#include <algorithm>

namespace pheet {

template <class Scheduler, size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic {
public:
	typedef ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE> Self;
	typedef ImprovedBranchBoundGraphBipartitioningSubproblem<Scheduler, Self, MAX_SIZE> Subproblem;

	ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic(Subproblem* sub_problem);
	ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic(Subproblem* sub_problem, Self const& other);
	~ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic();

	void init_root();
	size_t get_next_vertex();
	size_t get_cut();
	size_t get_lower_bound();
	size_t get_upper_bound();
	void update(uint8_t set, size_t pos);
	void bulk_update(uint8_t set, std::bitset<MAX_SIZE> positions);

	static void print_name();
private:
	Subproblem* sub_problem;

	size_t cut;
	size_t lb;
	size_t nv;
	size_t contrib_sum;

	size_t* weights[2];
	size_t* contributions;
};

template <class Scheduler, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic(Subproblem* sub_problem)
: sub_problem(sub_problem), cut(0), lb(0), nv(0), contrib_sum(0) {
	weights[0] = new size_t[sub_problem->size];
	weights[1] = new size_t[sub_problem->size];
	memset(weights[0], 0, sizeof(size_t)*sub_problem->size);
	memset(weights[1], 0, sizeof(size_t)*sub_problem->size);
	contributions = new size_t[sub_problem->size];

	size_t best_contrib = 0;
	size_t best_contrib_i = 0;
	for(size_t i = 0; i < sub_problem->size; ++i) {
		contributions[i] = 0;
		for(size_t j = 0; j < sub_problem->graph[i].num_edges; ++j) {
			contributions[i] += sub_problem->graph[i].edges[j].weight;
		}
		contrib_sum += contributions[i];
		if(contributions[i] > best_contrib) {
			best_contrib = contributions[i];
			best_contrib_i = i;
		}
	}
	nv = best_contrib_i;
	contrib_sum >>= 1;
}

template <class Scheduler, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic(Subproblem* sub_problem, Self const& other)
: sub_problem(sub_problem), cut(other.cut), lb(other.lb), nv(other.nv), contrib_sum(other.contrib_sum) {
	weights[0] = new size_t[sub_problem->size];
	weights[1] = new size_t[sub_problem->size];
	memcpy(weights[0], other.weights[0], sizeof(size_t)*sub_problem->size);
	memcpy(weights[1], other.weights[1], sizeof(size_t)*sub_problem->size);
	contributions = new size_t[sub_problem->size];
	memcpy(contributions, other.contributions, sizeof(size_t)*sub_problem->size);
}

template <class Scheduler, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::~ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic() {
	delete[] weights[0];
	delete[] weights[1];
	delete[] contributions;
}

template <class Scheduler, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::init_root() {

}

template <class Scheduler, size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::get_next_vertex() {
	assert(sub_problem->sets[2].test(nv));
	return nv;
}

template <class Scheduler, size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::get_cut() {
	return cut;
}

template <class Scheduler, size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::get_lower_bound() {
	return get_cut() + lb;
}

template <class Scheduler, size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::get_upper_bound() {
	return get_cut() + lb + contrib_sum;
}

template <class Scheduler, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::update(uint8_t set, size_t pos) {
	cut += weights[set ^ 1][pos];

	for(size_t i = 0; i < sub_problem->graph[pos].num_edges; ++i) {
		weights[set][sub_problem->graph[pos].edges[i].target] += sub_problem->graph[pos].edges[i].weight;
		if(sub_problem->sets[2].test(sub_problem->graph[pos].edges[i].target)) {
			assert(contributions[sub_problem->graph[pos].edges[i].target] >= sub_problem->graph[pos].edges[i].weight);
			contributions[sub_problem->graph[pos].edges[i].target] -= sub_problem->graph[pos].edges[i].weight;
			contrib_sum -= sub_problem->graph[pos].edges[i].weight;
		}
	}

	ptrdiff_t* delta = new ptrdiff_t[sub_problem->sets[2].count()];

	size_t di = 0;
	lb = 0;
	size_t current_bit = sub_problem->sets[2]._Find_first();
	nv = current_bit;
	size_t best_contrib = 0;
	while(current_bit != sub_problem->sets[2].size()) {
		ptrdiff_t d = (ptrdiff_t)weights[1][current_bit] - (ptrdiff_t)weights[0][current_bit];
		if(std::abs(d) + contributions[current_bit] >= best_contrib) {
			best_contrib = std::abs(d) + contributions[current_bit];
			nv = current_bit;
		}
		delta[di++] = d;
		lb += std::min(weights[0][current_bit], weights[1][current_bit]);
		current_bit = sub_problem->sets[2]._Find_next(current_bit);
	}
	std::sort(delta, delta + di);

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
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::bulk_update(uint8_t set, std::bitset<MAX_SIZE> positions) {
	size_t current_bit = positions._Find_first();
	while(current_bit != positions.size()) {
		update(set, current_bit);
		current_bit = positions._Find_next(current_bit);
	}
}

template <class Scheduler, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic<Scheduler, MAX_SIZE>::print_name() {
	std::cout << "DeltaContribNVLogic";
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTACONTRIBNVLOGIC_H_ */
