/*
 * PrioritySchedulerTaskExecutionContext.h
 *
 *  Created on: 19.09.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef PRIORITYSCHEDULERTASKEXECUTIONCONTEXT_H_
#define PRIORITYSCHEDULERTASKEXECUTIONCONTEXT_H_

#include "../../../settings.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../common/FinishRegion.h"
#include "../../../misc/atomics.h"
#include "../../../misc/bitops.h"
#include "../../../misc/type_traits.h"
#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Time/TimePerformanceCounter.h"

#include <vector>
#include <assert.h>
#include <iostream>

namespace pheet {

struct PrioritySchedulerPerformanceCounters {
	PrioritySchedulerPerformanceCounters() {}
	PrioritySchedulerPerformanceCounters(PrioritySchedulerPerformanceCounters& other)
		: num_spawns(other.num_spawns), num_spawns_to_call(other.num_spawns_to_call),
		  num_calls(other.num_calls), num_finishes(other.num_finishes),
		  num_steals(other.num_steals), num_steal_calls(other.num_steal_calls),
		  num_unsuccessful_steal_calls(other.num_unsuccessful_steal_calls),
		  num_stealing_deque_pop_cas(other.num_stealing_deque_pop_cas),
		  total_time(other.total_time), task_time(other.task_time),
		  idle_time(other.idle_time) {}

	BasicPerformanceCounter<scheduler_count_spawns> num_spawns;
	BasicPerformanceCounter<scheduler_count_spawns_to_call> num_spawns_to_call;
	BasicPerformanceCounter<scheduler_count_calls> num_calls;
	BasicPerformanceCounter<scheduler_count_finishes> num_finishes;

	BasicPerformanceCounter<stealing_deque_count_steals> num_steals;
	BasicPerformanceCounter<stealing_deque_count_steal_calls> num_steal_calls;
	BasicPerformanceCounter<stealing_deque_count_unsuccessful_steal_calls> num_unsuccessful_steal_calls;
	BasicPerformanceCounter<stealing_deque_count_pop_cas> num_stealing_deque_pop_cas;

	TimePerformanceCounter<scheduler_measure_total_time> total_time;
	TimePerformanceCounter<scheduler_measure_task_time> task_time;
	TimePerformanceCounter<scheduler_measure_idle_time> idle_time;
};

struct PrioritySchedulerTaskExecutionContextStackElement {
	// Modified by local thread. Incremented when task is spawned, decremented when finished
	size_t num_spawned;

	// Only modified by other threads. Stolen tasks that were finished (including spawned tasks)
	size_t num_finished_remote;

	// Pointer to num_finished_remote of another thread (the one we stole tasks from)
	PrioritySchedulerTaskExecutionContextStackElement* parent;
};

template <class TaskExecutionContext>
struct PrioritySchedulerTaskExecutionContextLevelDescription {
	TaskExecutionContext** partners;
	procs_t num_partners;
	procs_t local_id;
	procs_t total_size;
};

template <class TaskExecutionContext>
struct PrioritySchedulerTaskExecutionContextDequeItem {
	PrioritySchedulerTaskExecutionContextDequeItem();

	typename TaskExecutionContext::Task* task;
	typename TaskExecutionContext::StackElement* stack_element;

	bool operator==(PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const;
	bool operator!=(PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const;
};

template <class TaskExecutionContext>
PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext>::PrioritySchedulerTaskExecutionContextDequeItem()
: task(NULL), stack_element(NULL) {

}

template <class TaskExecutionContext>
bool PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext>::operator==(PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const {
	return other.task == task;
}

template <class TaskExecutionContext>
bool PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext>::operator!=(PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const {
	return other.task != task;
}


template <class TaskExecutionContext>
class nullable_traits<PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext> > {
public:
	static PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const null_value;
};

template <class TaskExecutionContext>
PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const nullable_traits<PrioritySchedulerTaskExecutionContextDequeItem<TaskExecutionContext> >::null_value;

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
class PrioritySchedulerTaskExecutionContext {
public:
	typedef PrioritySchedulerTaskExecutionContextLevelDescription<PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy> > LevelDescription;
	typedef typename Scheduler::Backoff Backoff;
	typedef typename Scheduler::CPUHierarchy CPUHierarchy;
	typedef typename Scheduler::Task Task;
	typedef PrioritySchedulerTaskExecutionContextStackElement StackElement;
	typedef PrioritySchedulerTaskExecutionContextDequeItem<PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy> > DequeItem;
	typedef TaskStorageT<DequeItem> TaskStorage;

	PrioritySchedulerTaskExecutionContext(std::vector<LevelDescription*> const* levels, std::vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state, PrioritySchedulerPerformanceCounters& perf_count);
	~PrioritySchedulerTaskExecutionContext();

	void join();

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams&& ... params);

	template<class CallTaskType, class Strategy, typename ... TaskParams>
		void spawn_prio(Strategy s, TaskParams&& ... params);

private:
	void run();
	void execute_task(Task* task, StackElement* parent);
	void main_loop();
	void process_queue();
	void wait_for_finish(StackElement* parent);

	void empty_stack();
	StackElement* create_non_blocking_finish_region(StackElement* parent);
	void signal_task_completion(StackElement* stack_element);
	void finalize_stack_element(StackElement* element, StackElement* parent);

	void start_finish_region();
	void end_finish_region();

	PrioritySchedulerPerformanceCounters performance_counters;

	static size_t const stack_size;
	StackElement* stack;
	StackElement* current_task_parent;
	size_t stack_filled_left;
	size_t stack_filled_right;

	LevelDescription* levels;
	procs_t num_levels;

	CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy> > thread_executor;

	typename Scheduler::State* scheduler_state;

	size_t max_queue_length;
	TaskStorage task_storage;

	friend class CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>>;
	friend class Scheduler::Finish;
};

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
size_t const PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::stack_size = 2048;

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::PrioritySchedulerTaskExecutionContext(std::vector<LevelDescription*> const* levels, std::vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state, PrioritySchedulerPerformanceCounters& perf_count)
: performance_counters(perf_count), stack_filled_left(0), stack_filled_right(stack_size), num_levels(levels->size()), thread_executor(cpus, this), scheduler_state(scheduler_state), max_queue_length(find_last_bit_set((*levels)[0]->total_size + 8) << 4), task_storage(max_queue_length, performance_counters.num_steals, performance_counters.num_stealing_deque_pop_cas) {
	performance_counters.total_time.start_timer();

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

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::~PrioritySchedulerTaskExecutionContext() {
	delete[] stack;
	delete[] levels;
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::join() {
	thread_executor.join();
}


template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::run() {
	scheduler_state->state_barrier.wait(0, 1);

	Task* startup_task = scheduler_state->startup_task;
	if(startup_task != NULL) {
		if(PTR_CAS(&(scheduler_state->startup_task), startup_task, NULL)) {
			execute_task(startup_task, NULL);
		}
	}
	main_loop();

	// We need to stop here, as it isn't safe after the barrier
	performance_counters.total_time.stop_timer();

	scheduler_state->state_barrier.barrier(1, levels[0].total_size);

	// Now we can safely finish execution
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::execute_task(Task* task, StackElement* parent) {
	if(parent < stack || (parent >= (stack + stack_size))) {
		// to prevent thrashing on the parent finish block (owned by another thread), we create a new finish block local to the thread

		// Create new stack element for finish
		parent = create_non_blocking_finish_region(parent);
	}

	// Store parent (needed for spawns inside the task)
	current_task_parent = parent;

	// Execute task
	performance_counters.task_time.start_timer();
	(*task)(*this);
	performance_counters.task_time.stop_timer();

	// Signal that we finished executing this task
	signal_task_completion(parent);
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::main_loop() {
	while(true) {
		// Make sure our queue is empty
		process_queue();

		{	// Local scope so we have a new backoff object
			Backoff bo;
			DequeItem di;
			performance_counters.idle_time.start_timer();
			while(true) {
				// Finalize elements in stack
				procs_t next_rand = random();

				// We do not steal from the last level as there are no partners
				procs_t level = num_levels - 1;
				while(level > 0) {
					level--;
					// For all except the last level we assume num_partners > 0
					assert(levels[level].num_partners > 0);
					assert(levels[level].partners[next_rand % levels[level].num_partners] != this);

					performance_counters.num_steal_calls.incr();
					di = levels[level].partners[next_rand % levels[level].num_partners]->task_storage.steal_push(this->task_storage);
				//	di = levels[level].partners[next_rand % levels[level].num_partners]->task_storage.steal();

					if(di.task != NULL) {
						break;
					}
					else {
						performance_counters.num_unsuccessful_steal_calls.incr();
					}
				}
				if(di.task == NULL) {
					if(scheduler_state->current_state >= 2) {
						performance_counters.idle_time.stop_timer();
						return;
					}
					bo.backoff();
				}
				else {
					performance_counters.idle_time.stop_timer();
					execute_task(di.task, di.stack_element);
					delete di.task;
					break;
				}
			}
		}
	}
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::wait_for_finish(StackElement* parent) {
	while(true) {
		// TODO: try a policy where we do not need to empty our queues before we notice the finish
		// (currently not implemented for simplicity)

		// Make sure our queue is empty
		process_queue();

		{	// Local scope so we have a new backoff object
			Backoff bo;
			DequeItem di;
			while(true) {
				// Finalize elements in stack
				procs_t next_rand = random();

				// We do not steal from the last level as there are no partners
				procs_t level = num_levels - 1;
				while(level > 0) {
					level--;
					// For all except the last level we assume num_partners > 0
					assert(levels[level].num_partners > 0);
					assert(levels[level].partners[next_rand % levels[level].num_partners] != this);
					performance_counters.num_steal_calls.incr();
					di = levels[level].partners[next_rand % levels[level].num_partners]->task_storage.steal_push(this->task_storage);
				//	di = levels[level].partners[next_rand % levels[level].num_partners]->task_storage.steal();

					if(di.task != NULL) {
						break;
					}
					else {
						performance_counters.num_unsuccessful_steal_calls.incr();
					}
				}
				if(di.task == NULL) {
					if(parent->num_finished_remote + 1 == parent->num_spawned) {
						return;
					}
					bo.backoff();
				}
				else {
					execute_task(di.task, di.stack_element);
					delete di.task;
					break;
				}
			}
		}
	}
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::process_queue() {
	DequeItem di = task_storage.pop();
	while(di.task != NULL) {
		// Warning, no distinction between locally spawned tasks and remote tasks
		// But this makes it easier with the finish construct, etc.
		// Otherwise we would have to empty our deque on the next finish call
		// which is bad for balancing
		execute_task(di.task, di.stack_element);
		delete di.task;
		di = task_storage.pop();
	}
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
typename PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::StackElement*
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::create_non_blocking_finish_region(StackElement* parent) {
	// Perform cleanup on finish stack
	empty_stack();

	assert(stack_filled_left < stack_filled_right);

	stack[stack_filled_left].num_finished_remote = 0;
	// As we create it out of a parent region that is waiting for completion of a single task, we can already add this one task here
	stack[stack_filled_left].num_spawned = 1;
	stack[stack_filled_left].parent = parent;

	++stack_filled_left;

	return &(stack[stack_filled_left - 1]);
}

/*
 * empty stack but not below limit
 */
template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::empty_stack() {
	while(stack_filled_left > 0) {
		size_t se = stack_filled_left - 1;
		if(stack[se].num_spawned == stack[se].num_finished_remote) {
	//		finalize_stack_element(&(stack[se]), stack[se].parent);

			stack_filled_left = se;

			// When parent is set to NULL, some thread is finalizing/has finalized this stack element (otherwise we would have an error)
			assert(stack[stack_filled_left].parent == NULL);
		}
		else {
			break;
		}
	}
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::signal_task_completion(StackElement* stack_element) {
	StackElement* parent = stack_element->parent;

	if(stack_element >= stack && (stack_element < (stack + stack_size))) {
		--(stack_element->num_spawned);

		// Memory fence is unfortunately required to guarantee that some thread finalizes the stack_element
		// TODO: prove that the fence is enough
		MEMORY_FENCE();
	}
	else {
		// Increment num_finished_remote of parent
		SIZET_ATOMIC_ADD(&(stack_element->num_finished_remote), 1);
	}
	if(stack_element->num_spawned == stack_element->num_finished_remote) {
		finalize_stack_element(stack_element, parent);
	}
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::finalize_stack_element(StackElement* element, StackElement* parent) {
	if(parent != NULL) {
		if(element->num_spawned == 0) {
			// No tasks processed remotely - no need for atomic ops
			element->parent = NULL;
			signal_task_completion(parent);
		}
		else {
			if(PTR_CAS(&(element->parent), parent, NULL)) {
				signal_task_completion(parent);
			}
		}
	}
	else {
		// Root element - we can shut down the scheduler
		scheduler_state->current_state = 2;
	}
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::start_finish_region() {
	performance_counters.task_time.stop_timer();
	performance_counters.num_finishes.incr();

	// Perform cleanup on left side of finish stack
	empty_stack();

	assert(stack_filled_left < stack_filled_right);
	--stack_filled_right;

	stack[stack_filled_right].num_finished_remote = 0;
	// We add 1 to make sure finishes are not propagated to the parent (as we wait manually and then execute a continuation)
	stack[stack_filled_right].num_spawned = 1;
	stack[stack_filled_right].parent = current_task_parent;

	current_task_parent = stack + stack_filled_right;
	performance_counters.task_time.start_timer();
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::end_finish_region() {
	performance_counters.task_time.stop_timer();
	assert(current_task_parent == &(stack[stack_filled_right]));

	if(current_task_parent->num_spawned > 1) {
		// There exist some non-executed or stolen tasks

		// Process other tasks until this task has been finished
		wait_for_finish(current_task_parent);
	}

	// Restore old parent
	current_task_parent = stack[stack_filled_right].parent;

	// Remove stack element
	++stack_filled_right;
	performance_counters.task_time.start_timer();
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::finish(TaskParams&& ... params) {
	start_finish_region();

	call<CallTaskType>(static_cast<TaskParams&&>(params) ...);

	end_finish_region();
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::spawn(TaskParams&& ... params) {
	spawn_prio<CallTaskType>(DefaultStrategy(), static_cast<TaskParams&&>(params) ...);
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
template<class CallTaskType, class Strategy, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::spawn_prio(Strategy s, TaskParams&& ... params) {
	performance_counters.num_spawns.incr();

	if(task_storage.get_length() >= max_queue_length) {
		performance_counters.num_spawns_to_call.incr();
		call<CallTaskType>(static_cast<TaskParams&&>(params) ...);
	}
	else {
		CallTaskType* task = new CallTaskType(params ...);
		assert(current_task_parent != NULL);
		++(current_task_parent->num_spawned);
		DequeItem di;
		di.task = task;
		di.stack_element = current_task_parent;
		task_storage.push(s, di);
	}
}

template <class Scheduler, template <typename T> class TaskStorageT, class DefaultStrategy>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy>::call(TaskParams&& ... params) {
	performance_counters.num_calls.incr();
	// Create task
	CallTaskType task(static_cast<TaskParams&&>(params) ...);
	// Execute task
	task(*this);
}

}

#endif /* PRIORITYSCHEDULERTASKEXECUTIONCONTEXT_H_ */
