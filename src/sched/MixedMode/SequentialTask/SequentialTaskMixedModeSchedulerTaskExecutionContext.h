/*
 * SequentialTaskMixedModeSchedulerTaskExecutionContext.h
 *
 *  Created on: 13.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SEQUENTIALTASKMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_
#define SEQUENTIALTASKMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_

#include "../../../../settings.h"

namespace pheet {

struct SequentialTaskMixedModeSchedulerTaskExecutionContextStackElement {
	// Modified by local thread. Incremented when task is spawned, decremented when finished
	size_t num_spawned;

	// Only modified by other threads. Stolen tasks that were finished (including spawned tasks)
	size_t num_finished_remote;

	// Pointer to num_finished_remote of another thread (the one we stole tasks from)
	size_t* parent;
};

template <class TaskExecutionContext>
struct SequentialTaskMixedModeSchedulerTaskExecutionContextLevelDescription {
	TaskExecutionContext* partners;
	procs_t num_partners;
	procs_t local_id;
	procs_t total_size;
};



template <class CPUHierarchy>
class SequentialTaskMixedModeSchedulerTaskExecutionContextInitializer {
	SequentialTaskMixedModeSchedulerTaskExecutionContextInitializer(CPUHierarchy& cpus, Task *startup_task);
};

template <class CPUHierarchy, class StealingDeque>
class SequentialTaskMixedModeSchedulerTaskExecutionContext {
public:
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContextLevelDescription<SequentialTaskMixedModeSchedulerTaskExecutionContext> LevelDescription;

	SequentialTaskMixedModeSchedulerTaskExecutionContext(vector<LevelDescription*>* levels, vector<CPUHierarchy::CPUDescriptor*>* cpus, SequentialTaskMixedModeScheduler::State* state);
	~SequentialTaskMixedModeSchedulerTaskExecutionContext();

	void run();
	void join();

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void local_finish(TaskParams ... params);
private:
	typedef SynchroneousMixedModeTask<SequentialTaskMixedModeScheduler> Task;
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContextStackElement StackElement;

	SequentialTaskMixedModeSchedulerTaskExecutionContext(CPUHierarchy& cpus);

	void initialize();
	void initialize_children();

	static size_t const stack_size;

	StackElement stack[stack_size];
	size_t stack_filled;
	Task* startup_task;
};

template <class CPUHierarchy, class StealingDeque>
size_t const SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::stack_size = 64;

template <class CPUHierarchy, class StealingDeque>
SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::SequentialTaskMixedModeSchedulerTaskExecutionContext()
: stack_filled(0), startup_task(startup_task) {

	if(PRE_INITIALIZE) {
		initialize();
		initialize_children();
	}
}

template <class CPUHierarchy, class StealingDeque>
SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::~SequentialTaskMixedModeSchedulerTaskExecutionContext() {

}

template <class CPUHierarchy, class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::run() {

}

template <class CPUHierarchy, class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::join() {

}


template <class CPUHierarchy, class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::initialize(CPUHierarchy& cpus, Task *startup_task) {

}

}

#endif /* SEQUENTIALTASKMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_ */
