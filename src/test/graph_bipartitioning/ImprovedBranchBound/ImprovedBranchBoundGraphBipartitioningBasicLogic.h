/*
 * ImprovedBranchBoundGraphBipartitioningBasicLogic.h
 *
 *  Created on: Dec 2, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGBASICLOGIC_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGBASICLOGIC_H_

#include "../../../settings.h"
#include "../graph_helpers.h"

#include <bitset>

namespace pheet {

template <size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioningBasicLogic {
public:
	typedef ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE> Self;
	typedef ImprovedBranchBoundGraphBipartitioningSubproblem<Self, MAX_SIZE> Subproblem;

	ImprovedBranchBoundGraphBipartitioningBasicLogic(Subproblem* sub_problem);
	ImprovedBranchBoundGraphBipartitioningBasicLogic(Subproblem* sub_problem, Self const& other);
	~ImprovedBranchBoundGraphBipartitioningBasicLogic();

	void init_root();
	size_t get_next_vertex();
	size_t get_cut();
	size_t get_lower_bound();
	void update(uint8_t set, size_t pos);
	void bulk_update(uint8_t set, std::bitset<MAX_SIZE> positions);

	static void print_name();
private:
	Subproblem* sub_problem;
};

template <size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningBasicLogic(Subproblem* sub_problem)
: sub_problem(sub_problem) {

}

template <size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningBasicLogic(Subproblem* sub_problem, Self const& other)
: sub_problem(sub_problem) {

}

template <size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::~ImprovedBranchBoundGraphBipartitioningBasicLogic() {

}

template <size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::init_root() {

}

template <size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::get_next_vertex() {
	return sub_problem->sets[2]._Find_first();
}

template <size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::get_cut() {
	size_t cut = 0;
	size_t current_bit = sub_problem->sets[0]._Find_first();
	while(current_bit != sub_problem->sets[0].size()) {
		for(size_t i = 0; i < sub_problem->graph[current_bit].num_edges; ++i) {
			if(sub_problem->sets[1].test(sub_problem->graph[current_bit].edges[i].target)) {
				cut += sub_problem->graph[current_bit].edges[i].weight;
			}
		}
		current_bit = sub_problem->sets[0]._Find_next(current_bit);
	}
	return cut;
}

template <size_t MAX_SIZE>
size_t ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::get_lower_bound() {
	return get_cut();
}

template <size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::update(uint8_t set, size_t pos) {

}

template <size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::bulk_update(uint8_t set, std::bitset<MAX_SIZE> positions) {
	size_t current_bit = positions._Find_first();
	while(current_bit != positions.size()) {
		update(set, current_bit);
		current_bit = positions._Find_next(current_bit);
	}
}

template <size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningBasicLogic<MAX_SIZE>::print_name() {
	std::cout << "BasicLogic";
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGBASICLOGIC_H_ */
