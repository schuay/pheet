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

template <class Task, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE = 64>
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

template <class Task, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
ImprovedStrategyBranchBoundGraphBipartitioningRootTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::ImprovedStrategyBranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution<MAX_SIZE>* out)
: graph(graph), size(size), out(out) {


}

template <class Task, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
ImprovedStrategyBranchBoundGraphBipartitioningRootTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::~ImprovedStrategyBranchBoundGraphBipartitioningRootTask() {

}

template <class Task, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
void ImprovedStrategyBranchBoundGraphBipartitioningRootTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::operator()(typename Task::TEC& tec) {
	MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> > best;
	size_t ub = std::numeric_limits< size_t >::max();

	size_t k = size >> 1;
	ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* prob = new ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>(graph, size, k);
	tec.template finish<BBTask>(prob, &ub, best);

	(*out) = best.get_max();
	assert(out->weight == ub);
	assert(ub != std::numeric_limits< size_t >::max());
	assert(out->weight != std::numeric_limits< size_t >::max());
	assert(out->sets[0].count() == k);
	assert(out->sets[1].count() == size - k);
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_ */