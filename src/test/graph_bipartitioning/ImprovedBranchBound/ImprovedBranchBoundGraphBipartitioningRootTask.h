/*
 * ImprovedImprovedBranchBoundGraphBipartitioningRootTask.h
 *
 *  Created on: Dec 2, 2011
 *      Author: mwimmer
 *	   License: Pheet license
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_

#include "ImprovedBranchBoundGraphBipartitioningTask.h"
#include "ImprovedBranchBoundGraphBipartitioningSubproblem.h"

#include <limits.h>

namespace pheet {

template <class Task, class Logic, size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioningRootTask : public Task {
public:
	typedef ImprovedBranchBoundGraphBipartitioningTask<Task, Logic, MAX_SIZE> BBTask;

	ImprovedBranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution<MAX_SIZE>* out);
	virtual ~ImprovedBranchBoundGraphBipartitioningRootTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	GraphVertex* graph;
	size_t size;
	GraphBipartitioningSolution<MAX_SIZE>* out;
};

template <class Task, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningRootTask<Task, Logic, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution<MAX_SIZE>* out)
: graph(graph), size(size), out(out) {


}

template <class Task, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningRootTask<Task, Logic, MAX_SIZE>::~ImprovedBranchBoundGraphBipartitioningRootTask() {

}

template <class Task, class Logic, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningRootTask<Task, Logic, MAX_SIZE>::operator()(typename Task::TEC& tec) {
	MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> > best;
	size_t ub = std::numeric_limits< size_t >::max();

	size_t k = size >> 1;
	ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* prob = new ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>(graph, size, k);
	tec.template finish<BBTask>(prob, &ub, best);

	(*out) = best.get_max();
	assert(out->weight != std::numeric_limits< size_t >::max());
	assert(out->sets[0].count() == k);
	assert(out->sets[1].count() == size - k);
}


}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_ */
