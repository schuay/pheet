/*
 * ImprovedBranchBoundGraphBipartitioningImprovedLogic.h
 *
 *  Created on: Dec 6, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGIMPROVEDLOGIC_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGIMPROVEDLOGIC_H_

#include "../../../settings.h"
#include "../graph_helpers.h"

#include <bitset>
#include <string.h>
#include <algorithm>

namespace pheet {

template <size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioningImprovedLogic {
public:
	typedef ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE> Self;
	typedef ImprovedBranchBoundGraphBipartitioningSubproblem<Self, MAX_SIZE> Subproblem;

	ImprovedBranchBoundGraphBipartitioningImprovedLogic(Subproblem* sub_problem);
	ImprovedBranchBoundGraphBipartitioningImprovedLogic(Subproblem* sub_problem, Self const& other);
	~ImprovedBranchBoundGraphBipartitioningImprovedLogic();

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

template <size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningImprovedLogic(Subproblem* sub_problem)
: sub_problem(sub_problem), cut(0), lb(0) {
	weights[0] = new size_t[sub_problem->size];
	weights[1] = new size_t[sub_problem->size];
	memset(weights[0], 0, sizeof(size_t)*sub_problem->size);
	memset(weights[1], 0, sizeof(size_t)*sub_problem->size);
}

template <size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningImprovedLogic(Subproblem* sub_problem, Self const& other)
: sub_problem(sub_problem), cut(other.cut), lb(other.lb) {
	weights[0] = new size_t[sub_problem->size];
	weights[1] = new size_t[sub_problem->size];
	memcpy(weights[0], other.weights[0], sizeof(size_t)*sub_problem->size);
	memcpy(weights[1], other.weights[1], sizeof(size_t)*sub_problem->size);
}

template <size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::~ImprovedBranchBoundGraphBipartitioningImprovedLogic() {
	delete[] weights[0];
	delete[] weights[1];
}

template <size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::init_root() {

}

template <size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::get_next_vertex() {
	return sub_problem->sets[2]._Find_first();
}

template <size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::get_cut() {
	return cut;
}

template <size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::get_lower_bound() {
	return get_cut() + lb;
}

template <size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::update(uint8_t set, size_t pos) {
	cut += weights[set ^ 1][pos];

	for(size_t i = 0; i < sub_problem->graph[pos].num_edges; ++i) {
		weights[set][sub_problem->graph[pos].edges[i].target] += sub_problem->graph[pos].edges[i].weight;
	}

	lb = 0;
	size_t current_bit = sub_problem->sets[2]._Find_first();
	while(current_bit != sub_problem->sets[2].size()) {
		lb += std::min(weights[0][current_bit], weights[1][current_bit]);
		current_bit = sub_problem->sets[2]._Find_next(current_bit);
	}
}

template <size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::bulk_update(uint8_t set, std::bitset<MAX_SIZE> positions) {
	size_t current_bit = positions._Find_first();
	while(current_bit != positions.size()) {
		update(set, current_bit);
		current_bit = positions._Find_next(current_bit);
	}
}

template <size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningImprovedLogic<MAX_SIZE>::print_name() {
	std::cout << "ImprovedLogic";
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGIMPROVEDLOGIC_H_ */
