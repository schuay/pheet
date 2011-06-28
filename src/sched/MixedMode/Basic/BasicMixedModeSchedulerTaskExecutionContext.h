/*
 * BasicMixedModeSchedulerTaskExecutionContext.h
 *
 *  Created on: 16.06.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef BASICMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_
#define BASICMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_

#include "../../../../settings.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../../misc/atomics.h"
#include "../../../misc/bitops.h"
#include "../../../misc/type_traits.h"

#include <vector>
#include <assert.h>
#include <iostream>

/*
 *
 */
namespace pheet {

struct BasicMixedModeSchedulerTaskExecutionContextStackElement {
	// Modified by local thread. Incremented when task is spawned, decremented when finished
	size_t num_spawned;

	// Only modified by other threads. Stolen tasks that were finished (including spawned tasks)
	size_t num_finished_remote;

	// Pointer to num_finished_remote of another thread (the one we stole tasks from)
	BasicMixedModeSchedulerTaskExecutionContextStackElement* parent;
};

template <class TaskExecutionContext>
struct BasicMixedModeSchedulerTaskExecutionContextLevelDescription {
	TaskExecutionContext** partners;
	procs_t num_partners;
	procs_t local_id;
	procs_t total_size;
};

union registration {
	uint64_t complete;
	struct {
		uint16_t c;	// counter
		uint16_t r;	// required threads
		uint16_t t;	// teamed threads
		uint16_t a;	// acquired threads
	} parts;
};

template <class TaskExecutionContext>
struct BasicMixedModeSchedulerTaskExecutionContextDequeItem {
	BasicMixedModeSchedulerTaskExecutionContextDequeItem();

	procs_t team_size;
	typename TaskExecutionContext::Task* task;
	typename TaskExecutionContext::StackElement* stack_element;

	bool operator==(BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const;
	bool operator!=(BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const;
};

template <class TaskExecutionContext>
BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext>::BasicMixedModeSchedulerTaskExecutionContextDequeItem()
: task(NULL), stack_element(NULL) {

}

template <class TaskExecutionContext>
bool BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext>::operator==(BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const {
	return other.task == task;
}

template <class TaskExecutionContext>
bool BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext>::operator!=(BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const {
	return other.task != task;
}


template <class TaskExecutionContext>
class nullable_traits<BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> > {
public:
	static BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const null_value;
};

template <class TaskExecutionContext>
BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const nullable_traits<BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> >::null_value;

template <class Scheduler, template <typename T> class StealingDeque>
class BasicMixedModeSchedulerTaskExecutionContext {
public:
	typedef BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> TaskExecutionContext;
	typedef BasicMixedModeSchedulerTaskExecutionContextLevelDescription<BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > LevelDescription;
	typedef typename Scheduler::Backoff Backoff;
	typedef typename Scheduler::CPUHierarchy CPUHierarchy;
	typedef typename Scheduler::Task Task;
	typedef BasicMixedModeSchedulerTaskExecutionContextStackElement StackElement;
	typedef BasicMixedModeSchedulerTaskExecutionContextDequeItem<BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > DequeItem;

	BasicMixedModeSchedulerTaskExecutionContext(vector<LevelDescription*> const* levels, vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state);
	~BasicMixedModeSchedulerTaskExecutionContext();

	void join();

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void finish_nt(procs_t nt_size, TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call_nt(procs_t nt_size, TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn_nt(procs_t nt_size, TaskParams ... params);

	void rebuild_team();

private:
	void run();
	void execute_task(Task* task, StackElement* parent);
	void main_loop();
	void process_queue();
	void empty_stack(size_t limit);

	void build_team(procs_t nt_size);
	void resize_team(procs_t nt_size);

	procs_t get_level_for_num_threads(procs_t num_threads);

	static size_t const stack_size;
	StackElement* stack;
	size_t stack_filled;

	LevelDescription* levels;
	procs_t num_levels;

	// Information about current team (local)
	TaskExecutionContext* coordinator;
	procs_t local_id;
	procs_t team_size;

	// Information whether current team is in sync (local)
	bool in_sync;

	// Each thread counts the tasks it has executed in this team (needed for barrier, etc.) (local)
	size_t num_executed_team_tasks;

	// All non-coordinators signal task start
	Barrier taskExecutionBarrier;

	// Next task to execute. null means we have finished executing the last task. (how do we distinguish it from cases where it hasn't been set yet. is this possible?)
	// Last task to finish executing the given task is responsible for updating num_finished_remote? (what are the implications. what if it is a "local" task)
	DequeItem* nextTask;


	CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > thread_executor;

	typename Scheduler::State* scheduler_state;

	StealingDeque<DequeItem> stealing_deque;

	friend class CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>>;
};

template <class Scheduler, template <typename T> class StealingDeque>
size_t const BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::stack_size = 256;

template <class Scheduler, template <typename T> class StealingDeque>
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::BasicMixedModeSchedulerTaskExecutionContext(vector<LevelDescription*> const* levels, vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state)
: stack_filled(0), num_levels(levels->size()), thread_executor(cpus, this), scheduler_state(scheduler_state), stealing_deque(find_last_bit_set((*levels)[0]->total_size - 1) << 4), coordinator(this), local_id(0), team_size(1), in_sync(true) {
	stack = new StackElement[stack_size];
	this->levels = new LevelDescription[num_levels];
	procs_t local_id = 0;
	for(ptrdiff_t i = num_levels - 1; i >= 0; i--) {
		this->levels[i].partners = (*levels)[i]->partners;
		this->levels[i].num_partners = (*levels)[i]->num_partners;
		local_id += (*levels)[i]->local_id;
		this->levels[i].local_id = local_id;
		this->levels[i].total_size = (*levels)[i]->total_size;
	}

	thread_executor.run();
}

template <class Scheduler, template <typename T> class StealingDeque>
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::~BasicMixedModeSchedulerTaskExecutionContext() {
	delete[] stack;
	delete[] levels;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::join() {
	thread_executor.join();
}


template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::run() {
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

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_task(Task* task, StackElement* parent) {
	assert(stack_filled < stack_size);

	stack[stack_filled].num_finished_remote = 0;
	stack[stack_filled].num_spawned = 0;
	stack[stack_filled].parent = parent;

	stack_filled++;

	(*task)(*this);
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::main_loop() {
	while(true) {
		// Make sure our queue is empty
		process_queue();

		{	// Local scope so we have a new backoff object
			Backoff bo;
			DequeItem di;
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
					assert(levels[level].partners[next_rand % levels[level].num_partners] != this);
					di = levels[level].partners[next_rand % levels[level].num_partners]->stealing_deque.steal_push(this->stealing_deque);
				//	di = levels[level].partners[next_rand % levels[level].num_partners]->stealing_deque.steal();

					if(di.task != NULL) {
						break;
					}
				}
				if(di.task == NULL) {
					if(scheduler_state->current_state >= 2) {
						return;
					}
					bo.backoff();
				}
				else {
					execute_task(di.task, di.stack_element);
					break;
				}
			}
		}
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::process_queue() {
	DequeItem di = stealing_deque.pop();
	while(di.task != NULL) {
		// Warning, no distinction between locally spawned tasks and remote tasks
		// But this makes it easier with the finish construct, etc.
		// Otherwise we would have to empty our deque on the next finish call
		// which is bad for balancing
		execute_task(di.task, di.stack_element);
		delete di.task;
		empty_stack(0);
		di = stealing_deque.pop();
	}
}

/*
 * empty stack but not below limit
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::empty_stack(size_t limit) {
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
				if(parent >= stack && (parent < (stack + stack_size))) {
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

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::rebuild_team() {
	if(!in_sync) {
		build_team(team_size);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::resize_team() {

}

/**
 * translate a number of threads to a level in the CPU hierarchy
 */
template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_level_for_num_threads(procs_t num_threads) {
	procs_t candidate = find_last_bit_set(num_threads);
	while(candidate < num_levels && levels[candidate].total_size < num_threads) {
		++candidate;
	}
	return candidate;
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish(TaskParams ... params) {
	rebuild_team();
	assert(stack_filled > 0);

	// Create task
	CallTaskType task(params ...);

	// Create a new stack element for new task
	// num_finished_remote is not required as this stack element blocks lower ones from finishing anyway
	execute_task(&task, NULL);
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::spawn(TaskParams ... params) {
	CallTaskType* task = new CallTaskType(params ...);
	assert(stack_filled > 0);
	stack[stack_filled - 1].num_spawned++;
	DequeItem di;
	di.task = task;
	di.stack_element = &(stack[stack_filled - 1]);
	di.team_size = team_size;
	stealing_deque.push(di);
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call(TaskParams ... params) {
	rebuild_team();
	CallTaskType task(params ...);
	task(*this);
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish_nt(procs_t nt_size, TaskParams ... params) {
	if(nt_size == team_size) {
		finish(params ...);
	}
	else {
		procs_t old_team = team_size;
		resize_team(nt_size);

		assert(stack_filled > 0);

		// Create task
		CallTaskType task(params ...);

		// Create a new stack element for new task
		// num_finished_remote is not required as this stack element blocks lower ones from finishing anyway
		execute_task(&task, NULL);

		if(team_size != old_team) {
			in_sync = false;
			team_size = old_team;
		}
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::spawn_nt(procs_t nt_size, TaskParams ... params) {
	if(nt_size == team_size) {
		spawn(params ...);
	}
	else {
		CallTaskType* task = new CallTaskType(params ...);
		assert(stack_filled > 0);
		stack[stack_filled - 1].num_spawned++;
		DequeItem di;
		di.task = task;
		di.stack_element = &(stack[stack_filled - 1]);
		di.team_size = team_size;
		stealing_deque.push(di);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call_nt(procs_t nt_size, TaskParams ... params) {
	if(nt_size == team_size) {
		call(params ...);
	}
	else {
		procs_t old_team = team_size;
		resize_team(nt_size);

		CallTaskType task(params ...);
		task(*this);

		if(team_size != old_team) {
			in_sync = false;
			team_size = old_team;
		}
	}
}


}

#endif /* BASICMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_ */
