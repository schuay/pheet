/*
 * BranchBoundGraphBipartitioningTask.h
 *
 *  Created on: 08.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef BRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_
#define BRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_

#include "../graph_helpers.h"
#include "../../../primitives/Reducer/Max/MaxReducer.h"
#include "../../../primitives/Backoff/Exponential/ExponentialBackoff.h"

#include <set>
#include <algorithm>

/*
 *
 */
namespace pheet {



template <class Task, class LowerBound, class NextVertex>
class BranchBoundGraphBipartitioningTask : public Task {
public:
	typedef BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex> BBTask;
	typedef ExponentialBackoff<> Backoff;

	BranchBoundGraphBipartitioningTask(GraphVertex* graph, size_t size, size_t k, MaxReducer<GraphBipartitioningSolution>& best, size_t* set1, size_t set1_size, size_t* set2, size_t set2_size, size_t* ub);
	virtual ~BranchBoundGraphBipartitioningTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	void prepare_solution();

	GraphVertex* graph;
	size_t size;
	size_t k;
	MaxReducer<GraphBipartitioningSolution> best;
	size_t* set1;
	size_t set1_size;
	size_t* set2;
	size_t set2_size;
	size_t* ub;
	size_t lb;

	LowerBound lb_calc;
	NextVertex nv_calc;
};

template <class Task, class LowerBound, class NextVertex>
BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex>::BranchBoundGraphBipartitioningTask(GraphVertex* graph, size_t size, size_t k, MaxReducer<GraphBipartitioningSolution>& best, size_t* set1, size_t set1_size, size_t* set2, size_t set2_size, size_t* ub)
: graph(graph), size(size), k(k), best(best), set1_size(set1_size), set2_size(set2_size), ub(ub) {
	this->set1 = new size_t[k];
	this->set2 = new size_t[size - k];

	for(size_t i = 0; i < set1_size; ++i) {
		this->set1[i] = set1[i];
	}
	if(set1_size > 0)
		std::sort(this->set1, this->set1 + set1_size);
	for(size_t i = 0; i < set2_size; ++i) {
		this->set2[i] = set2[i];
	}
	if(set2_size > 0)
		std::sort(this->set2, this->set2 + set2_size);

	lb = lb_calc(graph, size, k, this->set1, set1_size, this->set2, set2_size);
}

template <class Task, class LowerBound, class NextVertex>
BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex>::~BranchBoundGraphBipartitioningTask() {
	delete[] set1;
	delete[] set2;
}

template <class Task, class LowerBound, class NextVertex>
void BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex>::operator()(typename Task::TEC& tec) {
	if(lb >= *ub) {
		return;
	}
	size_t next = nv_calc(graph, size, k, set1, set1_size, set2, set2_size);

	set1[set1_size] = next;
	if(set1_size == k - 1) {
		++set1_size;
		prepare_solution();
		--set1_size;
	}
	else {
//		size_t sub_lb = lb_calc(graph, size, k, set1, set1_size, set2, set2_size);
	//	if(sub_lb < *ub) {
			tec.template spawn<BBTask>(graph, size, k, best, set1, set1_size + 1, set2, set2_size, ub);
	//	}
	}

	set2[set2_size] = next;
	if(set2_size == (size - k) - 1) {
		++set2_size;
		prepare_solution();
		--set2_size;
	}
	else {
	//	size_t sub_lb = lb_calc(graph, size, k, set1, set1_size, set2, set2_size);
	//	if(sub_lb < *ub) {
			tec.template spawn<BBTask>(graph, size, k, best, set1, set1_size, set2, set2_size + 1, ub);
	//	}
	}
}

template <class Task, class LowerBound, class NextVertex>
void BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex>::prepare_solution() {
	size_t* unfinished;
	size_t unfinished_size;
	GraphBipartitioningSolution sol;
	sol.weight = 0;

	if(set1_size == k) {
		unfinished = set2;
		unfinished_size = set2_size;
	}
	else {
		unfinished = set1;
		unfinished_size = set1_size;
	}
	size_t s1_i = 0;
	size_t s2_i = 0;
	for(size_t i = 0; i < size; ++i) {
		while(s1_i < set1_size && set1[s1_i] < i) {
			++s1_i;
		}
		while(s2_i < set2_size && set2[s2_i] < i) {
			++s2_i;
		}
		if((s1_i == set1_size || set1[s1_i] > i) &&
			(s2_i == set2_size || set2[s2_i] > i)) {
			unfinished[unfinished_size] = i;
			++unfinished_size;
		}
	}
	for(size_t i = 0; i < set1_size; ++i) {
		size_t node = set1[i];
		size_t i2 = i + 1;
		for(size_t j = 0; j < graph[node].num_edges; ++j) {
			size_t target = graph[node].edges[j].target;
			while(i2 < set1_size && set1[i2] < target) {
				++i2;
			}
			if(i2 == set1_size || set1[i2] > target) {
				sol.weight += graph[node].edges[j].weight;
			}
		}
	}
	for(size_t i = 0; i < set2_size; ++i) {
		size_t node = set2[i];
		size_t i2 = i + 1;
		for(size_t j = 0; j < graph[node].num_edges; ++j) {
			size_t target = graph[node].edges[j].target;
			while(i2 < set2_size && set2[i2] < target) {
				++i2;
			}
			if(i2 == set2_size || set2[i2] > target) {
				sol.weight += graph[node].edges[j].weight;
			}
		}
	}


	Backoff bo;
	while(true) {
		size_t old_ub = *ub;
		if(old_ub > sol.weight) {
			if(SIZET_CAS(ub, old_ub, sol.weight)) {
				best.add_value(sol);
				break;
			}
			bo.backoff();
		}
		else {
			break;
		}
	}
}

}

#endif /* BRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_ */
