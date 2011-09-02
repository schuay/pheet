/*
 * BasicMixedModeScheduler.h
 *
 *  Created on: 16.06.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef BASICMIXEDMODESCHEDULER_H_
#define BASICMIXEDMODESCHEDULER_H_

#include "../../common/SchedulerTask.h"
#include "BasicMixedModeSchedulerTaskExecutionContext.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../../em/CPUHierarchy/BinaryTree/BinaryTreeCPUHierarchy.h"

#include <stdint.h>
#include <limits>
#include <vector>

/*
 *
 */
namespace pheet {

template <class Task, class Barrier>
struct BasicMixedModeSchedulerState {
	BasicMixedModeSchedulerState();

	procs_t team_size;
	uint8_t current_state;
	Barrier state_barrier;
	Task *startup_task;
};

template <class Task, class Barrier>
BasicMixedModeSchedulerState<Task, Barrier>::BasicMixedModeSchedulerState()
: current_state(0), startup_task(NULL) {

}

/*
 * May only be used once
 */
template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
class BasicMixedModeScheduler {
public:
	typedef BackoffT Backoff;
	typedef CPUHierarchyT CPUHierarchy;
	typedef SchedulerTask<BasicMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff> > Task;
	typedef BasicMixedModeSchedulerTaskExecutionContext<BasicMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff>, StealingDeque> TaskExecutionContext;
	typedef BasicMixedModeSchedulerState<Task, Barrier> State;
	typedef FinishRegion<BasicMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff> > Finish;

	/*
	 * CPUHierarchyT must be accessible throughout the lifetime of the scheduler
	 * (Although the current implementation only uses it for initialization)
	 */
	BasicMixedModeScheduler(CPUHierarchyT* cpus);
	~BasicMixedModeScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
	void finish_nt(procs_t nt, TaskParams ... params);

	procs_t get_max_team_size();

	void print_performance_counter_values();

	static void print_performance_counter_headers();

	static char const name[];
	static procs_t const max_cpus;
private:
	void initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* cpus, size_t offset, vector<typename TaskExecutionContext::LevelDescription*>* levels);

	BinaryTreeCPUHierarchy<CPUHierarchy> cpu_hierarchy;
	TaskExecutionContext** threads;
	procs_t num_threads;

	State state;

	BasicMixedModeSchedulerPerformanceCounters performance_counters;
};

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
char const BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::name[] = "BasicMixedModeScheduler";

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
procs_t const BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::BasicMixedModeScheduler(CPUHierarchy* cpus)
: cpu_hierarchy(cpus), num_threads(cpus->get_size()) {

	threads = new TaskExecutionContext*[num_threads];

	vector<typename TaskExecutionContext::LevelDescription*> levels;
	initialize_tecs(&cpu_hierarchy, 0, &levels);
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::~BasicMixedModeScheduler() {
	for(procs_t i = 0; i < num_threads; i++) {
		delete threads[i];
	}
	delete[] threads;
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
void BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* ch, size_t offset, vector<typename TaskExecutionContext::LevelDescription*>* levels) {
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
			ld.reverse_ids = false;

			levels->push_back(&ld);
			initialize_tecs(sub0, offset, levels);
			ld.local_id = sub0->get_size();
			ld.num_partners = ld.local_id;
			ld.partners = threads + offset;
			ld.reverse_ids = true;
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
		ld.reverse_ids = false;
		levels->push_back(&ld);
		TaskExecutionContext *tec = new TaskExecutionContext(levels, ch->get_cpus(), &state, performance_counters);
		threads[offset] = tec;
		levels->pop_back();
	}
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::finish(TaskParams ... params) {
	finish_nt<CallTaskType, TaskParams ...>((procs_t)1, params ...);
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::finish_nt(procs_t nt, TaskParams ... params) {
	CallTaskType* task = new CallTaskType(params ...);
	state.team_size = nt;
	state.startup_task = task;
	state.current_state = 1;

	// signal scheduler threads that execution may start
	state.state_barrier.signal(0);

	for(procs_t i = 0; i < num_threads; i++) {
		threads[i]->join();
	}
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
procs_t BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::get_max_team_size() {
	return num_threads;
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
void BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::print_performance_counter_values() {

}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
void BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::print_performance_counter_headers() {

}

}

#endif /* BASICMIXEDMODESCHEDULER_H_ */
