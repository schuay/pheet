/*
 * ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic.h
 *
 *  Created on: Jan 13, 2012
 *      Author: Martin Wimmer, JLT
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTACONTRIBNVFREELOGIC_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTACONTRIBNVFREELOGIC_H_

#include "pheet/pheet.h"
#include "../graph_helpers.h"

#include <string.h>
#include <algorithm>

namespace pheet {

template <class Pheet, class SubProblem>
class ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic {
public:
	typedef ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem> Self;
	typedef typename SubProblem::Set Set;

	ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic(SubProblem* sub_problem);
	ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic(SubProblem* sub_problem, Self const& other);
	~ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic();

	void init_root();
	size_t get_next_vertex();
	size_t get_cut();
	size_t get_lower_bound();
	size_t get_estimate();
	size_t get_upper_bound();
	void update(uint8_t set, size_t pos);
	void bulk_update(uint8_t set, Set positions);

	static void print_name();
private:
	SubProblem* sub_problem;

	size_t cut;
	size_t lb;
	size_t nv;
	size_t ub;
	size_t est;
	size_t contrib_sum;
	size_t lb_ub_contrib;

	size_t* weights[2];
	size_t* contributions;
	size_t* free_edges; // JLT - number of free edges (into sets[2]) for each node
};

template <class Pheet, class SubProblem>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic(SubProblem* sub_problem)
: sub_problem(sub_problem), cut(0), lb(0), nv(0), ub(0), est(0), contrib_sum(0) {
	weights[0] = new size_t[sub_problem->size];
	weights[1] = new size_t[sub_problem->size];
	memset(weights[0], 0, sizeof(size_t)*sub_problem->size);
	memset(weights[1], 0, sizeof(size_t)*sub_problem->size);
	contributions = new size_t[sub_problem->size];
	free_edges = new size_t[sub_problem->size];

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
		free_edges[i] = sub_problem->graph[i].num_edges; // all edges free
	}
	nv = best_contrib_i;
	contrib_sum >>= 1;
}

template <class Pheet, class SubProblem>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic(SubProblem* sub_problem, Self const& other)
: sub_problem(sub_problem), cut(other.cut), lb(other.lb), nv(other.nv), ub(other.ub), est(other.est), contrib_sum(other.contrib_sum) {
	weights[0] = new size_t[sub_problem->size];
	weights[1] = new size_t[sub_problem->size];
	memcpy(weights[0], other.weights[0], sizeof(size_t)*sub_problem->size);
	memcpy(weights[1], other.weights[1], sizeof(size_t)*sub_problem->size);
	contributions = new size_t[sub_problem->size];
	memcpy(contributions, other.contributions, sizeof(size_t)*sub_problem->size);
	free_edges = new size_t[sub_problem->size];
	memcpy(free_edges,other.free_edges,sizeof(size_t)*sub_problem->size);
}

template <class Pheet, class SubProblem>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::~ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic() {
	delete[] weights[0];
	delete[] weights[1];
	delete[] contributions;
	delete[] free_edges;
}

template <class Pheet, class SubProblem>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::init_root() {

}

template <class Pheet, class SubProblem>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::get_next_vertex() {
	pheet_assert(sub_problem->sets[2].test(nv));
	return nv;
}

template <class Pheet, class SubProblem>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::get_cut() {
	return cut;
}

template <class Pheet, class SubProblem>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::get_lower_bound() {
	return get_cut() + lb;
}

template <class Pheet, class SubProblem>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::get_estimate() {
	return get_cut() + est + contrib_sum;
}

template <class Pheet, class SubProblem>
size_t ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::get_upper_bound() {
	return get_cut() + ub + contrib_sum;
}

template <class Pheet, class SubProblem>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::update(uint8_t set, size_t pos) {
	cut += weights[set ^ 1][pos];

	for(size_t i = 0; i < sub_problem->graph[pos].num_edges; ++i) {
		weights[set][sub_problem->graph[pos].edges[i].target] += sub_problem->graph[pos].edges[i].weight;
		if(sub_problem->sets[2].test(sub_problem->graph[pos].edges[i].target)) {
			pheet_assert(contributions[sub_problem->graph[pos].edges[i].target] >= sub_problem->graph[pos].edges[i].weight);
			contributions[sub_problem->graph[pos].edges[i].target] -= sub_problem->graph[pos].edges[i].weight;
			contrib_sum -= sub_problem->graph[pos].edges[i].weight;
			free_edges[sub_problem->graph[pos].edges[i].target]--;
		}
	}

	ptrdiff_t* delta = new ptrdiff_t[sub_problem->sets[2].count()];

	size_t di = 0;
	lb = 0;
	est = 0;
	ub = 0;
	lb_ub_contrib = 0;
	size_t current_bit = sub_problem->sets[2]._Find_first();
	nv = current_bit;
	size_t best_contrib = 0;
	// go through free nodes
	//size_t freedeg[sub_problem->size];
	size_t lastfree[2][sub_problem->size];
	while(current_bit != sub_problem->sets[2].size()) {
		ptrdiff_t d = (ptrdiff_t)weights[1][current_bit] - (ptrdiff_t)weights[0][current_bit];
		if(std::abs(d) + contributions[current_bit] >= best_contrib) {
			best_contrib = std::abs(d) + contributions[current_bit];
			nv = current_bit;
		}

		size_t v = current_bit; // the free vertex

		// possible contribution to delta
		int f = 0;
		size_t j = 0;
		size_t fw[2];
		size_t subsize[2], subrem[2];
		subsize[0] = sub_problem->k;
		subsize[1] = sub_problem->size-sub_problem->k;
		subrem[0] = subsize[0]-sub_problem->sets[0].count();
		subrem[1] = subsize[1]-sub_problem->sets[1].count();

		fw[0]= 0; fw[1] = 0;

		size_t s = (subrem[0]>subrem[1]) ? 0 : 1;

		while (f<(int)free_edges[v]-(int)subrem[s]) {
		  if (sub_problem->sets[2].test(sub_problem->graph[v].edges[j].target)) {
		    // free edge
		    if (sub_problem->graph[v].edges[j].target>v||sub_problem->graph[v].edges[j].reverse>=lastfree[0][sub_problem->graph[v].edges[j].target]) {
		      // not counted before
		      fw[0] += sub_problem->graph[v].edges[j].weight;
		      fw[1] += sub_problem->graph[v].edges[j].weight;
		    }
		    f++;
		  }
		  j++;
		}
		lastfree[s][v] = j;
		lastfree[1-s][v] = j;

		while (f<(int)free_edges[v]-(int)subrem[1-s]) {
		  if (sub_problem->sets[2].test(sub_problem->graph[v].edges[j].target)) {
		    if (sub_problem->graph[v].edges[j].target>v||sub_problem->graph[v].edges[j].reverse>=lastfree[0][sub_problem->graph[v].edges[j].target]) {
		      fw[1-s] += sub_problem->graph[v].edges[j].weight;
		    }
		    f++;
		  }
		  j++;
		}
		lastfree[1-s][v] = j;

		lb += std::min(fw[0],fw[1]);
		
		//JLT - brings something!! correct?
		//if (std::min(fw[0],fw[1])>0) std::cout<<'+';

		delta[di++] = d;//+((int)fw[0]-(int)fw[1]);
		lb += std::min(weights[0][current_bit], weights[1][current_bit]);
		est += std::min(weights[0][current_bit], weights[1][current_bit]);
		ub += std::max(weights[0][current_bit], weights[1][current_bit]);
		current_bit = sub_problem->sets[2]._Find_next(current_bit);
	}
	std::sort(delta, delta + di);

	size_t pivot = (/*sub_problem->size - */sub_problem->k) - sub_problem->sets[0].count();
	if(pivot < di && delta[pivot] < 0) {
		do {
			lb += -delta[pivot];
			est += -delta[pivot];
			ub -= -delta[pivot];
			++pivot;
		} while(pivot < di && delta[pivot] < 0);
	}
	else if(pivot > 0 && delta[pivot - 1] > 0) {
		do {
			--pivot;
			lb += delta[pivot];
			est += delta[pivot];
			ub += delta[pivot];
		} while(pivot > 0 && delta[pivot - 1] > 0);
	}

	delete[] delta;
}

template <class Pheet, class SubProblem>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::bulk_update(uint8_t set, Set positions) {
	size_t current_bit = positions._Find_first();
	while(current_bit != positions.size()) {
		update(set, current_bit);
		current_bit = positions._Find_next(current_bit);
	}
}

template <class Pheet, class SubProblem>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::print_name() {
	std::cout << "DeltaContribNVFREELogic";
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTACONTRIBNVFREELOGIC_H_ */
