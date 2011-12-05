/*
 * ImprovedStrategyBranchBoundGraphBipartitioningRootTask.h
 *
 *  Created on: Dec 5, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_

#include "ImprovedStrategyBranchBoundGraphBipartitioningTask.h"
#include "../ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningSubproblem.h"

#include <limits.h>

namespace pheet {

template <class Task, class Logic, template <class SubProblem> class SchedulingStrategy, size_t MAX_SIZE = 64>
class ImprovedStrategyBranchBoundGraphBipartitioningRootTask : public Task {
public:
	typedef ImprovedStrategyBranchBoundGraphBipartitioningTask<Task, Logic, SchedulingStrategy, MAX_SIZE> BBTask;

	ImprovedStrategyBranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution<MAX_SIZE>* out);
	virtual ~ImprovedStrategyBranchBoundGraphBipartitioningRootTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	GraphVertex* graph;
	size_t size;
	GraphBipartitioningSolution<MAX_SIZE>* out;
};

template <class Task, class Logic, template <class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
ImprovedStrategyBranchBoundGraphBipartitioningRootTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::ImprovedStrategyBranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution<MAX_SIZE>* out)
: graph(graph), size(size), out(out) {


}

template <class Task, class Logic, template <class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
ImprovedStrategyBranchBoundGraphBipartitioningRootTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::~ImprovedStrategyBranchBoundGraphBipartitioningRootTask() {

}

template <class Task, class Logic, template <class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
void ImprovedStrategyBranchBoundGraphBipartitioningRootTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::operator()(typename Task::TEC& tec) {
	MaxReducer<GraphBipartitioningSolution<MAX_SIZE> > best;
	size_t ub = std::numeric_limits< size_t >::max();

	size_t k = size >> 1;
	ImprovedBranchBoundGraphBipartitioningSubproblem<Logic, MAX_SIZE>* prob = new ImprovedBranchBoundGraphBipartitioningSubproblem<Logic, MAX_SIZE>(graph, size, k);
	tec.template finish<BBTask>(prob, &ub, best);

	(*out) = best.get_max();
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_ */
