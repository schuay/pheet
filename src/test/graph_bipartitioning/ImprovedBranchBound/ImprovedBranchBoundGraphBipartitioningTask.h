/*
 * ImprovedImprovedBranchBoundGraphBipartitioningTask.h
 *
 *  Created on: Dec 2, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_
#define IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_

#include "ImprovedBranchBoundGraphBipartitioningSubproblem.h"

namespace pheet {

template <class Task, class Logic, size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioningTask : public Task {
public:
	typedef ImprovedBranchBoundGraphBipartitioningTask<Task, Logic, MAX_SIZE> BBTask;
	typedef ExponentialBackoff<> Backoff;

	ImprovedBranchBoundGraphBipartitioningTask(ImprovedBranchBoundGraphBipartitioningSubproblem<Logic, MAX_SIZE>* sub_problem, size_t* upper_bound, MaxReducer<GraphBipartitioningSolution<MAX_SIZE> >& best);
	virtual ~ImprovedBranchBoundGraphBipartitioningTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	ImprovedBranchBoundGraphBipartitioningSubproblem<Logic, MAX_SIZE>* sub_problem;
	size_t* upper_bound;
	MaxReducer<GraphBipartitioningSolution<MAX_SIZE> > best;
};

template <class Task, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningTask<Task, Logic, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningTask(ImprovedBranchBoundGraphBipartitioningSubproblem<Logic, MAX_SIZE>* sub_problem, size_t* upper_bound, MaxReducer<GraphBipartitioningSolution<MAX_SIZE> >& best)
: sub_problem(sub_problem), upper_bound(upper_bound), best(best) {

}

template <class Task, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningTask<Task, Logic, MAX_SIZE>::~ImprovedBranchBoundGraphBipartitioningTask() {
	if(sub_problem != NULL) {
		delete sub_problem;
	}
}

template <class Task, class Logic, size_t MAX_SIZE>
void ImprovedBranchBoundGraphBipartitioningTask<Task, Logic, MAX_SIZE>::operator()(typename Task::TEC& tec) {
	if(sub_problem->get_lower_bound() >= *upper_bound) {
		return;
	}

	ImprovedBranchBoundGraphBipartitioningSubproblem<Logic, MAX_SIZE>* sub_problem2 =
			sub_problem->split();

	if(sub_problem->is_solution()) {
		sub_problem->update_solution(upper_bound, best);
	}
	else if(sub_problem->get_lower_bound() < *upper_bound) {
		tec.template spawn<BBTask>(sub_problem, upper_bound, best);
		// Make sure subproblem doesn't get deleted in destructor
		sub_problem = NULL;
	}

	if(sub_problem2->is_solution()) {
		sub_problem2->update_solution(upper_bound, best);
		delete sub_problem2;
	}
	else if(sub_problem2->get_lower_bound() < *upper_bound) {
		tec.template spawn<BBTask>(sub_problem2, upper_bound, best);
	}
	else {
		delete sub_problem2;
	}
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_ */
