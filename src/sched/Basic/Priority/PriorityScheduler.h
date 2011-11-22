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
#include <iostream>

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
template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold = 4>
class PriorityScheduler {
public:
	typedef BackoffT Backoff;
	typedef CPUHierarchyT CPUHierarchy;
	typedef SchedulerTask<PriorityScheduler<CPUHierarchy, TaskStorage, Barrier, Backoff, DefaultStrategy, CallThreshold> > Task;
	typedef PrioritySchedulerTaskExecutionContext<PriorityScheduler<CPUHierarchy, TaskStorage, Barrier, Backoff, DefaultStrategy, CallThreshold>, TaskStorage, DefaultStrategy, CallThreshold> TaskExecutionContext;
	typedef PrioritySchedulerState<Task, Barrier> State;
	typedef FinishRegion<PriorityScheduler<CPUHierarchy, TaskStorage, Barrier, Backoff, DefaultStrategy, CallThreshold> > Finish;

	/*
	 * CPUHierarchyT must be accessible throughout the lifetime of the scheduler
	 * (Although the current implementation only uses it for initialization)
	 */
	PriorityScheduler(CPUHierarchyT* cpus);
	~PriorityScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams&& ... params);

	static void print_name();

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

	typename TaskExecutionContext::PerformanceCounters performance_counters;
};

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
char const PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::name[] = "PriorityScheduler";

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
procs_t const PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::PriorityScheduler(CPUHierarchy* cpus)
: cpu_hierarchy(cpus), num_threads(cpus->get_size()) {

	threads = new TaskExecutionContext*[num_threads];

	std::vector<typename TaskExecutionContext::LevelDescription*> levels;
	initialize_tecs(&cpu_hierarchy, 0, &levels);
}

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::~PriorityScheduler() {

}

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
void PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* ch, size_t offset, std::vector<typename TaskExecutionContext::LevelDescription*>* levels) {
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

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
void PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::finish(TaskParams&& ... params) {
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

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
void PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::print_name() {
	std::cout << name << "<";
	TaskExecutionContext::TaskStorage::print_name();
	std::cout << ", " << (int)CallThreshold << ">";
}

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
void PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::print_performance_counter_values() {
	performance_counters.print_values();
}

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
void PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::print_performance_counter_headers() {
	performance_counters.print_headers();
}

template <class CPUHierarchyT, template <typename T> class TaskStorage, class Barrier, class BackoffT, class DefaultStrategy, uint8_t CallThreshold>
typename PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::TaskExecutionContext* PriorityScheduler<CPUHierarchyT, TaskStorage, Barrier, BackoffT, DefaultStrategy, CallThreshold>::get_context() {
	return TaskExecutionContext::get();
}

}


#endif /* PRIORITYSCHEDULER_H_ */
