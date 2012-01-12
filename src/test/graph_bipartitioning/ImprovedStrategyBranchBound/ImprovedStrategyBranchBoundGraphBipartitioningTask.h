/*
 * ImprovedStrategyBranchBoundGraphBipartitioningTask.h
 *
 *  Created on: Dec 5, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_
#define IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_

#include "../ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningSubproblem.h"
#include "ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters.h"

namespace pheet {

template <class Task, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE = 64>
class ImprovedStrategyBranchBoundGraphBipartitioningTask : public Task {
public:
	typedef ImprovedStrategyBranchBoundGraphBipartitioningTask<Task, Logic, SchedulingStrategy, MAX_SIZE> BBTask;
	typedef ExponentialBackoff<> Backoff;

	ImprovedStrategyBranchBoundGraphBipartitioningTask(ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* sub_problem, size_t* upper_bound, MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> >& best, ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<typename Task::Scheduler>& pc);
	virtual ~ImprovedStrategyBranchBoundGraphBipartitioningTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* sub_problem;
	size_t* upper_bound;
	MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> > best;
	SchedulingStrategy<typename Task::Scheduler, ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE> > strategy;
	ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<typename Task::Scheduler> pc;
};

template <class Task, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
ImprovedStrategyBranchBoundGraphBipartitioningTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::ImprovedStrategyBranchBoundGraphBipartitioningTask(ImprovedBranchBoundGraphBipartitioningSubproblem<typename Task::Scheduler, Logic, MAX_SIZE>* sub_problem, size_t* upper_bound, MaxReducer<typename Task::Scheduler, GraphBipartitioningSolution<MAX_SIZE> >& best, ImprovedStrategyBranchBoundGraphBipartitioningPerformanceCounters<typename Task::Scheduler>& pc)
: sub_problem(sub_problem), upper_bound(upper_bound), best(best), pc(pc) {

}

template <class Task, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
ImprovedStrategyBranchBoundGraphBipartitioningTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::~ImprovedStrategyBranchBoundGraphBipartitioningTask() {
	if(sub_problem != NULL) {
		delete sub_problem;
	}
}

template <class Task, class Logic, template <class Scheduler, class SubProblem> class SchedulingStrategy, size_t MAX_SIZE>
void ImprovedStrategyBranchBoundGraphBipartitioningTask<Task, Logic, SchedulingStrategy, MAX_SIZE>::operator()(typename Task::TEC& tec) {
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
		tec.template spawn_prio<BBTask>(strategy(sub_problem, upper_bound),
				sub_problem, upper_bound, best, pc);
		// Make sure subproblem doesn't get deleted in destructor
		sub_problem = NULL;
	}

	if(sub_problem2->is_solution()) {
		sub_problem2->update_solution(upper_bound, best, pc.subproblem_pc);
		delete sub_problem2;
	}
	else if(sub_problem2->get_lower_bound() < *upper_bound) {
		tec.template spawn_prio<BBTask>(strategy(sub_problem2, upper_bound),
				sub_problem2, upper_bound, best, pc);
	}
	else {
		delete sub_problem2;
	}
}

}

#endif /* IMPROVEDSTRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGTASK_H_ */
