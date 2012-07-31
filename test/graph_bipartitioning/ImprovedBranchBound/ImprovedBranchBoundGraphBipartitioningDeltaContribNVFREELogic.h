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
	size_t* scanned[2];
	size_t* seen[2];
	size_t* fweight[2];
};

template <class Pheet, class SubProblem>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic(SubProblem* sub_problem)
: sub_problem(sub_problem), cut(0), lb(0), nv(0), contrib_sum(0) {
  for (int i=0; i<2; i++) {
    weights[i] = new size_t[sub_problem->size];
    memset(weights[i], 0, sizeof(size_t)*sub_problem->size); // don't like
    scanned[i] = new size_t[sub_problem->size];
    seen[i]  = new size_t[sub_problem->size];
    fweight[i] = new size_t[sub_problem->size];
  }
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
    for (int j=0; j<2; j++) {
      scanned[j][i] = 0;
      seen[j][i]  = 0;
      fweight[j][i] = 0;
    }
  }
  nv = best_contrib_i;
  contrib_sum >>= 1;
}

template <class Pheet, class SubProblem>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic(SubProblem* sub_problem, Self const& other)
: sub_problem(sub_problem), cut(other.cut), lb(other.lb), nv(other.nv), contrib_sum(other.contrib_sum) {
    for (int i=0; i<2; i++) {
      weights[i] = new size_t[sub_problem->size];
      memcpy(weights[i], other.weights[i], sizeof(size_t)*sub_problem->size);
      scanned[i] = new size_t[sub_problem->size];
      memcpy(scanned[i], other.scanned[i], sizeof(size_t)*sub_problem->size);
      seen[i] = new size_t[sub_problem->size];
      memcpy(seen[i], other.seen[i], sizeof(size_t)*sub_problem->size);
      fweight[i] = new size_t[sub_problem->size];
      memcpy(fweight[i], other.fweight[i], sizeof(size_t)*sub_problem->size);
    }
    contributions = new size_t[sub_problem->size];
    memcpy(contributions, other.contributions, sizeof(size_t)*sub_problem->size);
    free_edges = new size_t[sub_problem->size];
    memcpy(free_edges,other.free_edges,sizeof(size_t)*sub_problem->size);
}

template <class Pheet, class SubProblem>
ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::~ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic() {
  for (int i=0; i<2; i++) {
    delete[] weights[i];
    delete[] scanned[i];
    delete[] seen[i];
    delete[] fweight[i];
  }
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
	pheet_assert(est + contrib_sum >= lb);
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

      for (size_t s=0; s<2; s++) {
	if (sub_problem->graph[pos].edges[i].reverse<scanned[s][sub_problem->graph[pos].edges[i].target]) {
	  pheet_assert(fweight[s][sub_problem->graph[pos].edges[i].target]>=sub_problem->graph[pos].edges[i].weight);

	  fweight[s][sub_problem->graph[pos].edges[i].target] -= sub_problem->graph[pos].edges[i].weight;
	  seen[s][sub_problem->graph[pos].edges[i].target]--;
	}
      }
    }
  }
  
  ptrdiff_t* delta = new ptrdiff_t[sub_problem->sets[2].count()];

  VertexWeight *smallest = new VertexWeight[sub_problem->sets[2].count()];

  size_t di = 0;
  size_t si = 0;
  lb = 0;
  est = 0;
  ub = 0;
  lb_ub_contrib = 0;
  size_t v = sub_problem->sets[2]._Find_first();
  nv = v;
  size_t best_contrib = 0;

  size_t f, j;
  size_t subrem[2];
  subrem[0] = sub_problem->k-sub_problem->sets[0].count();
  subrem[1] = (sub_problem->size-sub_problem->k)-sub_problem->sets[1].count();
  
  pheet_assert(subrem[0]+subrem[1]==sub_problem->sets[2].count());

  size_t ss = (subrem[0] < subrem[1]) ? 0 : 1;

  // go through free nodes
  while(v != sub_problem->sets[2].size()) {
    ptrdiff_t d = (ptrdiff_t)weights[1][v] - (ptrdiff_t)weights[0][v];
    if(std::abs(d) + contributions[v] >= best_contrib) {
      best_contrib = std::abs(d) + contributions[v];
      nv = v;
    }

    delta[di++] = d;
    lb += std::min(weights[0][v], weights[1][v]);
    est += std::min(weights[0][v], weights[1][v]);
    ub += std::max(weights[0][v], weights[1][v]);
    
    pheet_assert(free_edges[v]<sub_problem->sets[2].count());

    for (size_t s=0; s<2; s++) {
      if (subrem[s]>0) {
	f = seen[s][v]; j = scanned[s][v];
	//comment in next two lines to deactive incremental computation
	f = 0; j = 0;
	fweight[s][v] = 0;
	while (f+subrem[s]<=free_edges[v]) { // note <=
	  if (sub_problem->sets[2].test(sub_problem->graph[v].edges[j].target)) {
	    fweight[s][v] += sub_problem->graph[v].edges[j].weight;
	    f++;
	  }
	  j++;
	}
	seen[s][v] = f;
	scanned[s][v] = j;
      }
    }
    smallest[si].weight = fweight[ss][v];
    smallest[si].source = v;
    si++;

    v = sub_problem->sets[2]._Find_next(v);
  }
  std::sort(delta, delta + di);
  
  //size_t pivot = (/*sub_problem->size - */sub_problem->k) - sub_problem->sets[0].count();
  size_t pivot = subrem[0];
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
      ub -= delta[pivot];
    } while(pivot > 0 && delta[pivot - 1] > 0);
  }
  
  delete[] delta;

  size_t fw = 0;
  /*
  v = sub_problem->sets[2]._Find_first();
  while(v != sub_problem->sets[2].size()) {
    fw += std::min(fweight[0][v],fweight[1][v]);

    v = sub_problem->sets[2]._Find_next(v);
  }
  */
  std::sort(smallest,smallest+si,weight_compare());

  for (size_t i=0; i<subrem[ss]; i++) fw += smallest[i].weight;
  for (size_t i=subrem[ss]; i<subrem[0]+subrem[1]; i++) 
    fw += fweight[1-ss][smallest[i].source];
  fw >>= 1;
  //JLT - brings something!!
  //if (fw>0) std::cout<<'+'<<' '<<fw<<' '<<sub_problem->sets[2].count()<<'\n';
    
  lb += fw;
  est += fw;
  // Is this correct
  ub -= fw;

  delete [] smallest;
}

template <class Pheet, class SubProblem>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::bulk_update(uint8_t set, Set positions) {
	size_t v = positions._Find_first();
	while(v != positions.size()) {
		update(set, v);
		v = positions._Find_next(v);
	}
}

template <class Pheet, class SubProblem>
void ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic<Pheet, SubProblem>::print_name() {
	std::cout << "DeltaContribNVFREELogic";
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGDELTACONTRIBNVFREELOGIC_H_ */
