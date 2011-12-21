/*
 * StrategyBranchBoundGraphBipartitioningRootTask.h
 *
 *  Created on: 08.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef STRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_
#define STRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_

#include "StrategyBranchBoundGraphBipartitioningTask.h"

#include <limits.h>

/*
 *
 */
namespace pheet {

template <class Task, class LowerBound, class NextVertex, class SchedulingStrategy, size_t MAX_SIZE>
class StrategyBranchBoundGraphBipartitioningRootTask : public Task {
public:
	typedef StrategyBranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE> BBTask;

	StrategyBranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution<MAX_SIZE>* out);
	virtual ~StrategyBranchBoundGraphBipartitioningRootTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	GraphVertex* graph;
	size_t size;
	GraphBipartitioningSolution<MAX_SIZE>* out;
};

template <class Task, class LowerBound, class NextVertex, class SchedulingStrategy, size_t MAX_SIZE>
StrategyBranchBoundGraphBipartitioningRootTask<Task, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::StrategyBranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution<MAX_SIZE>* out)
: graph(graph), size(size), out(out) {


}

template <class Task, class LowerBound, class NextVertex, class SchedulingStrategy, size_t MAX_SIZE>
StrategyBranchBoundGraphBipartitioningRootTask<Task, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::~StrategyBranchBoundGraphBipartitioningRootTask() {

}

template <class Task, class LowerBound, class NextVertex, class SchedulingStrategy, size_t MAX_SIZE>
void StrategyBranchBoundGraphBipartitioningRootTask<Task, LowerBound, NextVertex, SchedulingStrategy, MAX_SIZE>::operator()(typename Task::TEC& tec) {
	MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> > best;
	size_t ub = std::numeric_limits< size_t >::max();

	size_t k = size >> 1;
	tec.template finish<BBTask>(graph, size, k, best, static_cast<size_t*>(new size_t[k]), 0, static_cast<size_t*>(new size_t[size - k]), 0, &ub, 0);

	(*out) = best.get_max();
}


}

#endif /* STRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_ */
