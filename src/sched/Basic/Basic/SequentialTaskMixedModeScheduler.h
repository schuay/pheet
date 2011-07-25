/*
 * SequentialTaskMixedModeScheduler.h
 *
 *  Created on: 13.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SEQUENTIALTASKMIXEDMODESCHEDULER_H_
#define SEQUENTIALTASKMIXEDMODESCHEDULER_H_

#include "SequentialTaskMixedModeSchedulerTaskExecutionContext.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../../em/CPUHierarchy/BinaryTree/BinaryTreeCPUHierarchy.h"

#include <stdint.h>
#include <limits>
#include <vector>

namespace pheet {

template <class Task, class Barrier>
struct SequentialTaskMixedModeSchedulerState {
	SequentialTaskMixedModeSchedulerState();

	uint8_t current_state;
	Barrier state_barrier;
	Task *startup_task;
};

template <class Task, class Barrier>
SequentialTaskMixedModeSchedulerState<Task, Barrier>::SequentialTaskMixedModeSchedulerState()
: current_state(0), startup_task(NULL) {

}

/*
 * May only be used once
 */
template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
class SequentialTaskMixedModeScheduler {
public:
	typedef BackoffT Backoff;
	typedef CPUHierarchyT CPUHierarchy;
	typedef SynchroneousMixedModeTask<SequentialTaskMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff> > Task;
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContext<SequentialTaskMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff>, StealingDeque> TaskExecutionContext;
	typedef SequentialTaskMixedModeSchedulerState<Task, Barrier> State;

	/*
	 * CPUHierarchyT must be accessible throughout the lifetime of the scheduler
	 * (Although the current implementation only uses it for initialization)
	 */
	SequentialTaskMixedModeScheduler(CPUHierarchyT* cpus);
	~SequentialTaskMixedModeScheduler();

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
};

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
char const SequentialTaskMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::name[] = "SequentialTaskMixedModeScheduler";

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
procs_t const SequentialTaskMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
SequentialTaskMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::SequentialTaskMixedModeScheduler(CPUHierarchy* cpus)
: cpu_hierarchy(cpus), num_threads(cpus->get_size()) {

	threads = new TaskExecutionContext*[num_threads];

	vector<typename TaskExecutionContext::LevelDescription*> levels;
	initialize_tecs(&cpu_hierarchy, 0, &levels);
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
SequentialTaskMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::~SequentialTaskMixedModeScheduler() {
	for(procs_t i = 0; i < num_threads; i++) {
		delete threads[i];
	}
	delete[] threads;
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
void SequentialTaskMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::initialize_tecs(BinaryTreeCPUHierarchy<CPUHierarchy>* ch, size_t offset, vector<typename TaskExecutionContext::LevelDescription*>* levels) {
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
		TaskExecutionContext *tec = new TaskExecutionContext(levels, ch->get_cpus(), &state);
		threads[offset] = tec;
		levels->pop_back();
	}
}

template <class CPUHierarchyT, template <typename T> class StealingDeque, class Barrier, class BackoffT>
template<class CallTaskType, typename ... TaskParams>
void SequentialTaskMixedModeScheduler<CPUHierarchyT, StealingDeque, Barrier, BackoffT>::finish(TaskParams ... params) {
	CallTaskType task(params ...);
	state.startup_task = &task;
	state.current_state = 1;

	// signal scheduler threads that execution may start
	state.state_barrier.signal(0);

	for(procs_t i = 0; i < num_threads; i++) {
		threads[i]->join();
	}
}

}


#endif /* SEQUENTIALTASKMIXEDMODESCHEDULER_H_ */
