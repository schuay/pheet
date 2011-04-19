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
#include "../../common/CPUThreadExecutor.h"
#include "../../../misc/atomics.h"

#include <vector>

namespace pheet {

struct SequentialTaskMixedModeSchedulerTaskExecutionContextStackElement {
	// Modified by local thread. Incremented when task is spawned, decremented when finished
	size_t num_spawned;

	// Only modified by other threads. Stolen tasks that were finished (including spawned tasks)
	size_t num_finished_remote;

	// Pointer to num_finished_remote of another thread (the one we stole tasks from)
	StackElement* parent;
};

template <class TaskExecutionContext>
struct SequentialTaskMixedModeSchedulerTaskExecutionContextLevelDescription {
	TaskExecutionContext* partners;
	procs_t num_partners;
	procs_t local_id;
	procs_t total_size;
};

template <class TaskExecutionContext>
struct SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem {
	SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem();

	TaskExecutionContext::Task* task;
	TaskExecutionContext::StackElement* stack_element;
};

template <class TaskExecutionContext>
SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext>::SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem()
: task(NULL), stack_element(NULL) {

}

template <class TaskExecutionContext>
class nullable_traits<SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> > {
	static SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const null_value;
};

template <class TaskExecutionContext>
SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const nullable_traits<SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> >::null_value;

}

template <class Scheduler, template <typename T> class StealingDeque>
class SequentialTaskMixedModeSchedulerTaskExecutionContext {
public:
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContextLevelDescription<SequentialTaskMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > LevelDescription;
	typedef Scheduler::Backoff Backoff;
	typedef Scheduler::CPUHierarchy CPUHierarchy;
	typedef Scheduler::Task Task;
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContextStackElement StackElement;
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContextDequeItem<SequentialTaskMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > DequeItem;

	SequentialTaskMixedModeSchedulerTaskExecutionContext(vector<LevelDescription*>* levels, vector<CPUHierarchy::CPUDescriptor*>* cpus, SequentialTaskMixedModeScheduler::State* scheduler_state);
	~SequentialTaskMixedModeSchedulerTaskExecutionContext();

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

	void execute_task(Task* task, size_t* parent);
	void main_loop();
	void process_queue();
	void empty_stack();

	static size_t const stack_size;
	StackElement stack[stack_size];
	size_t stack_filled;

	LevelDescription* levels;
	procs_t num_levels;

	CPUThreadExecutor<CPUHierarchy::CPUDescriptor, SequentialTaskMixedModeSchedulerTaskExecutionContext> thread_executor;

	SequentialTaskMixedModeScheduler::State* scheduler_state;

	size_t max_queue_length;
	StealingDeque<DequeItem> stealing_deque;

	friend class CPUThreadExecutor<CPUHierarchy::CPUDescriptor, SequentialTaskMixedModeSchedulerTaskExecutionContext>;
};

template <class CPUHierarchy, template <typename T> class StealingDeque>
size_t const SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::stack_size = 64;

template <class CPUHierarchy, template <typename T> class StealingDeque>
SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::SequentialTaskMixedModeSchedulerTaskExecutionContext(vector<LevelDescription*>* levels, vector<CPUHierarchy::CPUDescriptor*>* cpus, SequentialTaskMixedModeScheduler::State* scheduler_state)
: stack_filled(0), num_levels(levels.size()), thread_executor(cpus, this), scheduler_state(state), max_queue_length(find_last_bit_set(levels[0]->total_size - 1) << 4), stealing_deque(max_queue_length) {
	this->levels = new LevelDescription[num_levels];
	procs_t local_id = 0;
	for(ptrdiff_t i = num_levels - 1; i >= 0; i--) {
		this->levels[i].partners = levels[i]->partners;
		this->levels[i].num_partners = levels[i]->num_partners;
		local_id += levels[i]->local_id;
		this->levels[i].local_id = local_id;
		this->levels[i].total_size = levels[i]->total_size;
	}

	thread_executor.run();
}

template <class CPUHierarchy, template <typename T> class StealingDeque>
SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::~SequentialTaskMixedModeSchedulerTaskExecutionContext() {
	delete[] levels;
}

template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::join() {
	thread_executor.join();
}


template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::run() {
	scheduler_state->state_barrier.wait(0, 1);

	Task* startup_task = scheduler_state->startup_task;
	if(startup_task != NULL) {
		if(PTR_CAS(&(scheduler_state->startup_task), startup_task, NULL)) {
			execute_task(startup_task, NULL);
		}
	}

	main_loop();

	scheduler_state->state_barrier.barrier(1, levels[0].total_size);
	// Now we can safely finish execution
}

template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::execute_task(Task* task, size_t* parent) {
	assert(stack_filled < stack_size);

	stack[stack_filled].num_finished_remote = 0;
	stack[stack_filled].num_spawned = 0;
	stack[stack_filled].parent = parent;

	stack_filled++;

	task->execute(*this);
}

template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::main_loop() {
	while(true) {
		// Make sure our queue is empty
		process_queue();

		{	// Local scope so we have a new backoff object
			Backoff bo;
			Task* task = NULL;
			while(true) {
				// Finalize elements in stack
				empty_stack(0);

				procs_t next_rand = random();

				// We do not steal from the last level as there are no partners
				procs_t level = num_levels - 1;
				while(level > 0) {
					level--;
					// For all except the last level we assume num_partners > 0
					assert(levels[level].num_partners > 0);

					task = levels[level].partners[next_rand % levels[level].num_partners].stealing_deque.steal_append(this->stealing_deque);

					if(task != NULL) {
						break;
					}
				}
				if(task == NULL) {
					if(scheduler_state->current_state >= 2) {
						return;
					}
					bo.backoff();
				}
				else {
					execute_task(task, NULL /* TODO: get parent stack element in an atomic manner */);
					break;
				}
			}
		}
	}
}

template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::process_queue() {
	DequeItem di = stealing_deque.pop();
	while(di.task != NULL) {
		// Warning, no distinction between locally spawned tasks and remote tasks
		// But this makes it easier with the finish construct, etc.
		// Otherwise we would have to empty our deque on the next finish call
		// which is bad for balancing
		execute_task(di.task, di.stack_element);
		delete di.task;
		task = stealing_deque.pop();
	}
}

/*
 * empty stack but not below limit
 */
template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::empty_stack(size_t limit) {
	while(stack_filled > limit) {
		size_t se = stack_filled - 1;
		if(stack[se].num_spawned == stack[se].num_finished_remote) {
			stack_filled = se;
			StackElement * parent = stack[stack_filled].parent;
			if(parent == NULL) {
				if(stack_filled == 0) {
					// We have finished the root task
					scheduler_state->current_state = 2; // finished
					return;
				}
				else {
					// some non-root task finished
				}
			}
			else {
				if(parent >= (&stack) && (parent < ((&stack) + stack_size))) {
					// Parent is actually local. No need for atomics
					parent->num_spawned--;
				}
				else {
					// Increment num_finished_remote of parent
					SIZET_ATOMIC_ADD(&(parent->num_finished_remote), 1);
				}
			}
		}
		else {
			break;
		}
	}
}

template<class CallTaskType, typename ... TaskParams>
template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::finish(TaskParams ... params) {
	assert(stack_filled > 0);

	// Create task
	CallTaskType task(params ...);

	// Create a new stack element for new task
	// num_finished_remote is not required as this stack element blocks lower ones from finishing anyway
	execute_task(&task, NULL);
}

template<class CallTaskType, typename ... TaskParams>
template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::spawn(TaskParams ... params) {
	if(stealing_deque.get_length() >= max_queue_length) {
		call<CallTaskType>(params ...);
	}
	else {
		CallTaskType* task = new CallTaskType(params ...);
		assert(stack_filled > 0);
		stack[stack_filled - 1].num_spawned++;
		DequeItem di;
		di.task = task;
		di.stack_element = &(stack[stack_filled - 1]);
		stealing_deque.push(di);
	}
}

template<class CallTaskType, typename ... TaskParams>
template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::call(TaskParams ... params) {
	CallTaskType task(params ...);
	task.execute(tec);
}

template<class CallTaskType, typename ... TaskParams>
template <class CPUHierarchy, template <typename T> class StealingDeque>
void SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque>::local_finish(TaskParams ... params) {
	finish<CallTaskType>(params ...);
}

}

#endif /* SEQUENTIALTASKMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_ */
