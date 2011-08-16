/*
 * BasicScheduler.h
 *
 *  Created on: 13.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef BASICSCHEDULER_H_
#define BASICSCHEDULER_H_

#include "../../common/SchedulerTask.h"
#include "../../common/FinishRegion.h"
#include "BasicSchedulerTaskExecutionContext.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../../em/CPUHierarchy/BinaryTree/BinaryTreeCPUHierarchy.h"

#include <stdint.h>
#include <limits>
#include <vector>

namespace pheet {

template <class Task, class Barrier>
struct BasicSchedulerState {
	BasicSchedulerState();

	uint8_t current_state;
	Barrier state_barrier;
	Task *startup_task;
};

template <class Task, class Barrier>
BasicSchedulerState<Task, Barrier>::BasicSchedulerState()
: current_state(0), startup_task(NULL) {

}

/*
 * May only be used once
 */
template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
class BasicScheduler {
public:
	typedef BackoffT Backoff;
	typedef CPUHierarchyT CPUHierarchy;
	typedef SchedulerTask<BasicScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff> > Task;
	typedef BasicSchedulerTaskExecutionContext<BasicScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff>, StealingDeque> TaskExecutionContext;
	typedef BasicSchedulerState<Task, Barrier> State;
	typedef FinishRegion<BasicScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff> > Finish;

	/*
	 * CPUHierarchyT must be accessible throughout the lifetime of the scheduler
	 * (Although the current implementation only uses it for initialization)
	 */
	BasicScheduler(CPUHierarchyT* cpus);
	~BasicScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams ... params);

	static char const name[];
	static procs_t const max_cpus;

private:
	void initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* cpus, size_t offset, vector<typename TaskExecutionContext::LevelDescription*>* levels);

	BinaryTreeCPUHierarchy<CPUHierarchy> cpu_hierarchy;
	TaskExecutionContext** threads;
	procs_t num_threads;

	State state;

	BasicSchedulerPerformanceCounters performance_counters;
};

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
char const BasicScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::name[] = "BasicScheduler";

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
procs_t const BasicScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
BasicScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::BasicScheduler(CPUHierarchy* cpus)
: cpu_hierarchy(cpus), num_threads(cpus->get_size()) {

	threads = new TaskExecutionContext*[num_threads];

	vector<typename TaskExecutionContext::LevelDescription*> levels;
	initialize_tecs(&cpu_hierarchy, 0, &levels);
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
BasicScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::~BasicScheduler() {

}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
void BasicScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* ch, size_t offset, vector<typename TaskExecutionContext::LevelDescription*>* levels) {
	if(ch->get_size() > 1) {
		vector<BinaryTreeCPUHierarchy<CPUHierarchy>*> const* sub = ch->get_subsets();

		if(sub->size() == 2) {
			BinaryTreeCPUHierarchy<CPUHierarchy>* sub0 = (*sub)[0];
			BinaryTreeCPUHierarchy<CPUHierarchy>* sub1 = (*sub)[1];

			typename TaskExecutionContext::LevelDescription ld;
			ld.total_size = ch->get_size();
			ld.local_id = 0;
			ld.num_partners = sub1->get_size();
			ld.partners = threads + offset + sub0->get_size();
			levels->push_back(&ld);
			initialize_tecs(sub0, offset, levels);
			ld.local_id = sub0->get_size();
			ld.num_partners = ld.local_id;
			ld.partners = threads + offset;
			initialize_tecs(sub1, offset + ld.local_id, levels);

			levels->pop_back();
		}
		else {
			BinaryTreeCPUHierarchy<CPUHierarchy>* sub0 = (*sub)[0];

			initialize_tecs(sub0, offset, levels);
		}
	}
	else {
		typename TaskExecutionContext::LevelDescription ld;
		ld.total_size = 1;
		ld.local_id = 0;
		ld.num_partners = 0;
		ld.partners = NULL;
		levels->push_back(&ld);
		TaskExecutionContext *tec = new TaskExecutionContext(levels, ch->get_cpus(), &state, performance_counters);
		threads[offset] = tec;
		levels->pop_back();
	}
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
template<class CallTaskType, typename ... TaskParams>
void BasicScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::finish(TaskParams ... params) {
	CallTaskType task(params ...);
	state.startup_task = &task;
	state.current_state = 1;

	// signal scheduler threads that execution may start
	state.state_barrier.signal(0);

	for(procs_t i = 0; i < num_threads; i++) {
		threads[i]->join();
	}

	for(procs_t i = 0; i < num_threads; i++) {
		delete threads[i];
	}
	delete[] threads;

	performance_counters.num_spawns.output("Num spawns: %d\n");
	performance_counters.num_calls.output("Num calls: %d\n");
	performance_counters.num_spawns_to_call.output("Num spawns converted to call: %d\n");
	performance_counters.num_finishes.output("Num finishes: %d\n");
	performance_counters.num_steals.output("Num stealen tasks: %d\n");
	performance_counters.num_steal_calls.output("Num steal calls: %d\n");
	performance_counters.num_unsuccessful_steal_calls.output("Num unsuccessful steal calls: %d\n");
	performance_counters.num_stealing_deque_pop_cas.output("Num Stealing deque pop CAS: %d\n");
	performance_counters.total_time.output("Total time (all threads): %f\n");
	performance_counters.task_time.output("Task time (all threads): %f\n");
	performance_counters.idle_time.output("Idle (stealing) time (all threads): %f\n");
}

}


#endif /* BASICSCHEDULER_H_ */
