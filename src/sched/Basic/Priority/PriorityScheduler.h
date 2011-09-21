/*
 * PriorityScheduler.h
 *
 *  Created on: 19.09.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef PRIORITYSCHEDULER_H_
#define PRIORITYSCHEDULER_H_

#include "../../common/SchedulerTask.h"
#include "../../common/FinishRegion.h"
#include "PrioritySchedulerTaskExecutionContext.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../../em/CPUHierarchy/BinaryTree/BinaryTreeCPUHierarchy.h"

#include <stdint.h>
#include <limits>
#include <vector>

namespace pheet {

template <class Task, class Barrier>
struct PrioritySchedulerState {
	PrioritySchedulerState();

	uint8_t current_state;
	Barrier state_barrier;
	Task *startup_task;
};

template <class Task, class Barrier>
PrioritySchedulerState<Task, Barrier>::PrioritySchedulerState()
: current_state(0), startup_task(NULL) {

}

/*
 * May only be used once
 */
template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
class PriorityScheduler {
public:
	typedef BackoffT Backoff;
	typedef CPUHierarchyT CPUHierarchy;
	typedef SchedulerTask<PriorityScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff, DefaultStrategy> > Task;
	typedef PrioritySchedulerTaskExecutionContext<PriorityScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff, DefaultStrategy>, StealingDeque, DefaultStrategy> TaskExecutionContext;
	typedef PrioritySchedulerState<Task, Barrier> State;
	typedef FinishRegion<PriorityScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff, DefaultStrategy> > Finish;

	/*
	 * CPUHierarchyT must be accessible throughout the lifetime of the scheduler
	 * (Although the current implementation only uses it for initialization)
	 */
	PriorityScheduler(CPUHierarchyT* cpus);
	~PriorityScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams&& ... params);

	void print_performance_counter_values();

	void print_performance_counter_headers();

	TaskExecutionContext* get_context();

	static char const name[];
	static procs_t const max_cpus;

private:
	void initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* cpus, size_t offset, std::vector<typename TaskExecutionContext::LevelDescription*>* levels);

	BinaryTreeCPUHierarchy<CPUHierarchy> cpu_hierarchy;
	TaskExecutionContext** threads;
	procs_t num_threads;

	State state;

	PrioritySchedulerPerformanceCounters performance_counters;
};

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
char const PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::name[] = "PriorityScheduler";

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
procs_t const PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::PriorityScheduler(CPUHierarchy* cpus)
: cpu_hierarchy(cpus), num_threads(cpus->get_size()) {

	threads = new TaskExecutionContext*[num_threads];

	std::vector<typename TaskExecutionContext::LevelDescription*> levels;
	initialize_tecs(&cpu_hierarchy, 0, &levels);
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::~PriorityScheduler() {

}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
void PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* ch, size_t offset, std::vector<typename TaskExecutionContext::LevelDescription*>* levels) {
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

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
template<class CallTaskType, typename ... TaskParams>
void PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::finish(TaskParams&& ... params) {
	CallTaskType task(static_cast<TaskParams&&>(params) ...);
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
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
void PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::print_performance_counter_values() {
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
	performance_counters.idle_time.print("%f\t");
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
void PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::print_performance_counter_headers() {
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
	TimePerformanceCounter<scheduler_measure_idle_time>::print_header("total_idle_time\t");
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT, class DefaultStrategy>
typename PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::TaskExecutionContext* PriorityScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT, DefaultStrategy>::get_context() {
	return TaskExecutionContext::get();
}

}


#endif /* PRIORITYSCHEDULER_H_ */
