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

namespace pheet {

struct SequentialTaskMixedModeSchedulerState {
	uint8_t current_state;
	Barrier state_barrier;
	Task *startup_task;
};

template <class CPUHierarchy, class StealingDeque, class Barrier, class Backoff>
class SequentialTaskMixedModeScheduler {
public:
	typedef SynchroneousMixedModeTask<SequentialTaskMixedModeScheduler> Task;
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque> TaskExecutionContext;
	typedef SequentialTaskMixedModeSchedulerState State;
	typedef Backoff Backoff;

	/*
	 * CPUHierarchy must be accessible throughout the lifetime of the scheduler
	 * (Although the current implementation only uses it for initialization)
	 */
	SequentialTaskMixedModeScheduler(CPUHierarchy* cpus);
	~SequentialTaskMixedModeScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams ... params);

	static char const name[];

private:
	void initialize_tecs(CPUHierarchy* cpus, size_t offset);
	void initialize_tecs_range(CPUHierarchy* cpus, size_t offset)

	BinaryTreeCPUHierarchy<CPUHierarchy> cpu_hierarchy;
	TaskExecutionContext** threads;
	procs_t num_threads;

	State state;
};

template <class CPUHierarchy, class StealingDeque, class Barrier, class Backoff>
char const SequentialTaskMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff>::name[] = "SequentialTaskMixedModeScheduler";


template <class CPUHierarchy, class StealingDeque, class Barrier, class Backoff>
SequentialTaskMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff>::SequentialTaskMixedModeScheduler(CPUHierarchy* cpus)
: cpu_hierarchy(cpus), num_threads(cpus.get_size()){

	threads = new TaskExecutionContext*[num_threads];

	vector<TaskExecutionContext::LevelDescription*> levels;
	initialize_tecs(&cpu_hierarchy, 0, &levels);
}

template <class CPUHierarchy, class StealingDeque, class Barrier, class Backoff>
SequentialTaskMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff>::~SequentialTaskMixedModeScheduler() {
	for(procs_t i = 0; i < num_threads; i++) {
		delete threads[i];
	}
	delete[] threads;
}

template <class CPUHierarchy, class StealingDeque, class Barrier, class Backoff>
void SequentialTaskMixedModeScheduler<CPUHierarchy, StealingDeque, Barrier, Backoff>::initialize_tecs(CPUHierarchy* ch, size_t offset, vector<TaskExecutionContext::LevelDescription*>* levels) {
	if(ch->get_size() > 1) {
		vector<BinaryTreeCPUHierarchy*>* sub = ch->get_subsets();

		if(sub.size() == 2) {
			BinaryTreeCPUHierarchy* sub0 = (*sub)[0];
			BinaryTreeCPUHierarchy* sub1 = (*sub)[1];

			TaskExecutionContext::LevelDescription ld;
			ld.total_size = ch->get_size();
			ld.local_id = 0;
			ld.num_partners = sub1->get_size();
			ld.partners = threads + offset;
			levels->push_back(&ld);
			initialize_tecs(sub0, offset, levels);
			ld.local_id = sub0->get_size();
			ld.num_partners = ld.local_id;
			ld.partners += ld.local_id;
			initialize_tecs(sub1, offset + ld.local_id, levels);

			levels->pop_back();
		}
		else {
			BinaryTreeCPUHierarchy* sub0 = (*sub)[0];

			initialize_tecs(sub0, offset, levels);
		}
	}
	else {
		TaskExecutionContext::LevelDescription ld;
		ld.total_size = 1;
		ld.local_id = 0;
		ld.num_partners = 0;
		ld.partners = NULL;
		levels.push_back(&ld);

		TaskExecutionContext *tec = new TaskExecutionContext(levels, ch->get_cpus(), &state);
		threads[offset] = tec;
		levels->pop_back();
	}
}


#endif /* SEQUENTIALTASKMIXEDMODESCHEDULER_H_ */
