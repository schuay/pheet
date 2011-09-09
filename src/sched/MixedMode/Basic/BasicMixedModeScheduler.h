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

	void print_performance_counter_headers();

	static char const name[];
	static procs_t const max_cpus;
private:
	void initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* cpus, size_t offset, std::vector<typename TaskExecutionContext::LevelDescription*>* levels);

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
: cpu_hierarchy(cpus), num_threads(cpus->get_size()),
  performance_counters(/* don't expect the compiler to understand that get_depth is side-effect free, so add this conditional statement */
		  scheduler_count_tasks_at_level?cpu_hierarchy.get_max_depth():0, cpus->get_size()) {

	threads = new TaskExecutionContext*[num_threads];

	std::vector<typename TaskExecutionContext::LevelDescription*> levels;
	initialize_tecs(&cpu_hierarchy, 0, &levels);
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::~BasicMixedModeScheduler() {

}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
void BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* ch, size_t offset, std::vector<typename TaskExecutionContext::LevelDescription*>* levels) {
	if(ch->get_size() > 1) {
		std::vector<BinaryTreeCPUHierarchy<CPUHierarchy>*> const* sub = ch->get_subsets();

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
		delete threads[i];
	}
	delete[] threads;
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
procs_t BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::get_max_team_size() {
	return num_threads;
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
void BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::print_performance_counter_values() {
	performance_counters.num_spawns.print("%d\t");
	performance_counters.num_calls.print("%d\t");
	performance_counters.num_spawns_to_call.print("%d\t");
	performance_counters.num_finishes.print("%d\t");
	performance_counters.num_steals.print("%d\t");
	performance_counters.num_steal_calls.print("%d\t");
	performance_counters.num_unsuccessful_steal_calls.print("%d\t");
	performance_counters.num_stealing_deque_pop_cas.print("%d\t");
	performance_counters.total_time.print("%f\t");
	performance_counters.task_time.print("%f\t");
	performance_counters.sync_time.print("%f\t");
	performance_counters.idle_time.print("%f\t");
	performance_counters.queue_processing_time.print("%f\t");
	performance_counters.visit_partners_time.print("%f\t");
	performance_counters.wait_for_finish_time.print("%f\t");
	performance_counters.wait_for_coordinator_time.print("%f\t");

	if(scheduler_count_tasks_at_level) {
		procs_t depth = cpu_hierarchy.get_max_depth();
		for(procs_t i = 0; i < depth; ++i) {
			performance_counters.num_tasks_at_level.print(i, "%d\t");
		}
	}
	if(scheduler_count_steal_calls_per_thread) {
		for(procs_t i = 0; i < num_threads; ++i) {
			performance_counters.num_steal_calls_per_thread.print(i, "%d\t");
		}
	}
	if(scheduler_count_unsuccessful_steal_calls_per_thread) {
		for(procs_t i = 0; i < num_threads; ++i) {
			performance_counters.num_unsuccessful_steal_calls_per_thread.print(i, "%d\t");
		}
	}
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
void BasicMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::print_performance_counter_headers() {
	BasicPerformanceCounter<scheduler_count_spawns>::print_header("spawns\t");
	BasicPerformanceCounter<scheduler_count_spawns_to_call>::print_header("calls\t");
	BasicPerformanceCounter<scheduler_count_calls>::print_header("spawns->call\t");
	BasicPerformanceCounter<scheduler_count_finishes>::print_header("finishes\t");

	BasicPerformanceCounter<stealing_deque_count_steals>::print_header("stolen\t");
	BasicPerformanceCounter<stealing_deque_count_steal_calls>::print_header("steal_calls\t");
	BasicPerformanceCounter<stealing_deque_count_unsuccessful_steal_calls>::print_header("unsuccessful_steal_calls\t");
	BasicPerformanceCounter<stealing_deque_count_pop_cas>::print_header("stealing_deque_pop_cas\t");

	TimePerformanceCounter<scheduler_measure_total_time>::print_header("scheduler_total_time\t");
	TimePerformanceCounter<scheduler_measure_task_time>::print_header("total_task_time\t");
	TimePerformanceCounter<scheduler_measure_sync_time>::print_header("total_sync_time\t");
	TimePerformanceCounter<scheduler_measure_idle_time>::print_header("total_idle_time\t");
	TimePerformanceCounter<scheduler_measure_queue_processing_time>::print_header("queue_processing_time\t");
	TimePerformanceCounter<scheduler_measure_visit_partners_time>::print_header("visit_partners_time\t");
	TimePerformanceCounter<scheduler_measure_wait_for_finish_time>::print_header("wait_for_finish_time\t");
	TimePerformanceCounter<scheduler_measure_wait_for_coordinator_time>::print_header("wait_for_coordinator_time\t");

	if(scheduler_count_tasks_at_level) {
		procs_t depth = cpu_hierarchy.get_max_depth();
	//	char header[32];
		for(procs_t i = 0; i < depth; ++i) {
			printf("tasks_at_level_%d\t", (int)i);
		}
	}
	if(scheduler_count_steal_calls_per_thread) {
		for(procs_t i = 0; i < num_threads; ++i) {
			printf("steal_calls_at_thread_%d\t", (int)i);
		}
	}
	if(scheduler_count_unsuccessful_steal_calls_per_thread) {
		for(procs_t i = 0; i < num_threads; ++i) {
			printf("unsuccessful_steal_calls_at_thread_%d\t", (int)i);
		}
	}
}

}

#endif /* BASICMIXEDMODESCHEDULER_H_ */
