/*
 * BranchBoundGraphBipartitioningRootTask.h
 *
 *  Created on: 08.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef BRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_
#define BRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_

#include "BranchBoundGraphBipartitioningTask.h"

#include <limits.h>

/*
 *
 */
namespace pheet {

template <class Task, class LowerBound, class NextVertex>
class BranchBoundGraphBipartitioningRootTask : public Task {
public:
	typedef BranchBoundGraphBipartitioningTask<Task, LowerBound, NextVertex> BBTask;

	BranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution* out);
	virtual ~BranchBoundGraphBipartitioningRootTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	GraphVertex* graph;
	size_t size;
	GraphBipartitioningSolution* out;
};

template <class Task, class LowerBound, class NextVertex>
BranchBoundGraphBipartitioningRootTask<Task, LowerBound, NextVertex>::BranchBoundGraphBipartitioningRootTask(GraphVertex* graph, size_t size, GraphBipartitioningSolution* out)
: graph(graph), size(size), out(out) {


}

template <class Task, class LowerBound, class NextVertex>
BranchBoundGraphBipartitioningRootTask<Task, LowerBound, NextVertex>::~BranchBoundGraphBipartitioningRootTask() {

}

template <class Task, class LowerBound, class NextVertex>
void BranchBoundGraphBipartitioningRootTask<Task, LowerBound, NextVertex>::operator()(typename Task::TEC& tec) {
	MaxReducer<GraphBipartitioningSolution> best;
	size_t ub = std::numeric_limits< size_t >::max();

	size_t k = size >> 1;
	tec.template finish<BBTask>(graph, size, k, best, static_cast<size_t*>(new size_t[k]), 0, static_cast<size_t*>(new size_t[size - k]), 0, &ub, 0);

	(*out) = best.get_max();
}


}

#endif /* BRANCHBOUNDGRAPHBIPARTITIONINGROOTTASK_H_ */
