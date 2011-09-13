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

/*
 *
 */
namespace pheet {



template <class Task, class LowerBound, class NextVertex>
class BranchBoundGraphBipartitioningTask : public Task {
public:
	typedef BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex> BBTask;
	typedef ExponentialBackoff<> Backoff;

	BranchBoundGraphBipartitioningTask(GraphVertex* graph, size_t size, size_t k, MaxReducer<GraphBipartitioningSolution>& best, std::set<size_t> set1, std::set<size_t> set2, size_t* ub, size_t lb);
	virtual ~BranchBoundGraphBipartitioningTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	void prepare_solution();

	GraphVertex* graph;
	size_t size;
	size_t k;
	MaxReducer<GraphBipartitioningSolution> best;
	std::set<size_t> set1;
	std::set<size_t> set2;
	size_t* ub;
	size_t lb;

	LowerBound lb_calc;
	NextVertex nv_calc;
};

template <class Task, class LowerBound, class NextVertex>
BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex>::BranchBoundGraphBipartitioningTask(GraphVertex* graph, size_t size, size_t k, MaxReducer<GraphBipartitioningSolution>& best, std::set<size_t> set1, std::set<size_t> set2, size_t* ub, size_t lb)
: graph(graph), size(size), k(k), best(best), set1(set1), set2(set2), ub(ub), lb(lb) {

}

template <class Task, class LowerBound, class NextVertex>
BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex>::~BranchBoundGraphBipartitioningTask() {

}

template <class Task, class LowerBound, class NextVertex>
void BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex>::operator()(typename Task::TEC& tec) {
	if(lb >= *ub) {
		return;
	}
	size_t next = nv_calc(graph, size, k, set1, set2);

	set1.insert(next);
	if(set1.size() == k) {
		prepare_solution();
	}
	else {
		size_t sub_lb = lb_calc(graph, size, k, set1, set2);
		if(sub_lb < *ub) {
			tec.template spawn<BBTask>(graph, size, k, best, set1, set2, ub, sub_lb);
		}
	}
	set1.erase(next);

	set2.insert(next);
	if(set2.size() == size - k) {
		prepare_solution();
	}
	else {
		size_t sub_lb = lb_calc(graph, size, k, set1, set2);
		if(sub_lb < *ub) {
			tec.template spawn<BBTask>(graph, size, k, best, set1, set2, ub, sub_lb);
		}
	}
}

template <class Task, class LowerBound, class NextVertex>
void BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex>::prepare_solution() {
	std::set<size_t>* unfinished;
	GraphBipartitioningSolution sol;
	sol.weight = 0;
	sol.set1 = set1;
	sol.set2 = set2;
	if(set1.size() == k) {
		unfinished = &(sol.set2);
	}
	else {
		unfinished = &(sol.set1);
	}
	for(size_t i = 0; i < size; ++i) {
		if(sol.set1.find(i) == sol.set1.end() && sol.set2.find(i) == sol.set2.end()) {
			unfinished->insert(i);
		}
	}
	for(size_t i = 0; i < size; ++i) {
		if(sol.set1.find(i) != sol.set1.end()) {
			for(size_t j = 0; j < graph[i].num_edges; ++j) {
				if(sol.set2.find(graph[i].edges[j].target) != sol.set2.end()) {
					sol.weight += graph[i].edges[j].weight;
				}
			}
		}
		else {
			for(size_t j = 0; j < graph[i].num_edges; ++j) {
				if(sol.set1.find(graph[i].edges[j].target) != sol.set1.end()) {
					sol.weight += graph[i].edges[j].weight;
				}
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
