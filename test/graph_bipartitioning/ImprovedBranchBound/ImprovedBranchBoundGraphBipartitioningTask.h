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
#include "ImprovedBranchBoundGraphBipartitioningPerformanceCounters.h"

namespace pheet {

template <class Task, class Logic, size_t MAX_SIZE = 64>
class ImprovedBranchBoundGraphBipartitioningTask : public Task {
public:
	typedef ImprovedBranchBoundGraphBipartitioningTask<Task, Logic, MAX_SIZE> BBTask;
	typedef ExponentialBackoff<> Backoff;

	ImprovedBranchBoundGraphBipartitioningTask(ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* sub_problem, size_t* upper_bound, MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> >& best, ImprovedBranchBoundGraphBipartitioningPerformanceCounters<typename Task::Scheduler>& pc);
	virtual ~ImprovedBranchBoundGraphBipartitioningTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* sub_problem;
	size_t* upper_bound;
	MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> > best;
	ImprovedBranchBoundGraphBipartitioningPerformanceCounters<typename Task::Scheduler> pc;
};

template <class Task, class Logic, size_t MAX_SIZE>
ImprovedBranchBoundGraphBipartitioningTask<Task, Logic, MAX_SIZE>::ImprovedBranchBoundGraphBipartitioningTask(ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* sub_problem, size_t* upper_bound, MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> >& best, ImprovedBranchBoundGraphBipartitioningPerformanceCounters<typename Task::Scheduler>& pc)
: sub_problem(sub_problem), upper_bound(upper_bound), best(best), pc(pc) {

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
		pc.num_irrelevant_tasks.incr();
		return;
	}

	ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* sub_problem2 =
			sub_problem->split(pc.subproblem_pc);

	if(sub_problem->is_solution()) {
		sub_problem->update_solution(upper_bound, best, pc.subproblem_pc);
	}
	else if(sub_problem->get_lower_bound() < *upper_bound) {
		tec.template spawn<BBTask>(sub_problem, upper_bound, best, pc);
		// Make sure subproblem doesn't get deleted in destructor
		sub_problem = NULL;
	}

	if(sub_problem2->is_solution()) {
		sub_problem2->update_solution(upper_bound, best, pc.subproblem_pc);
		delete sub_problem2;
	}
	else if(sub_problem2->get_lower_bound() < *upper_bound) {
		tec.template call<BBTask>(sub_problem2, upper_bound, best, pc);
	}
	else {
		delete sub_problem2;
	}
}

}

#endif /* IMPROVEDBRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_ */
