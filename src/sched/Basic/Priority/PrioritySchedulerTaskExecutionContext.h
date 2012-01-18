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
#include "PrioritySchedulerPerformanceCounters.h"

#include <vector>
#include <assert.h>
#include <iostream>

#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>

namespace pheet {

struct PrioritySchedulerTaskExecutionContextStackElement {
	// Modified by local thread. Incremented when task is spawned, decremented when finished
	size_t num_spawned;

	// Only modified by other threads. Stolen tasks that were finished (including spawned tasks)
	size_t num_finished_remote;

	// Pointer to num_finished_remote of another thread (the one we stole tasks from)
	PrioritySchedulerTaskExecutionContextStackElement* parent;

	// Counter to update atomically when finalizing this element (ABA problem)
	// Unused on the right side
	size_t version;

	// When a finish hasn't yet spawned any tasks, we don't need to create a new block for the next level
	// instead we just increment this counter
	// unused on the left side
	size_t reused_finish_depth;
};

template <class TaskExecutionContext>
struct PrioritySchedulerTaskExecutionContextLevelDescription {
	TaskExecutionContext** partners;
	procs_t num_partners;
	procs_t local_id;
	procs_t total_size;
	procs_t memory_level;
	procs_t global_id_offset;
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

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
class PrioritySchedulerTaskExecutionContext {
public:
	typedef PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold> Self;
	typedef PrioritySchedulerTaskExecutionContextLevelDescription<Self> LevelDescription;
	typedef typename Scheduler::Backoff Backoff;
	typedef typename Scheduler::CPUHierarchy CPUHierarchy;
	typedef typename Scheduler::Task Task;
	typedef PrioritySchedulerTaskExecutionContextStackElement StackElement;
	typedef PrioritySchedulerTaskExecutionContextDequeItem<PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold> > DequeItem;
	typedef TaskStorageT<Scheduler, DequeItem> TaskStorage;
	typedef PrioritySchedulerPerformanceCounters<Scheduler, typename TaskStorage::PerformanceCounters> PerformanceCounters;

	PrioritySchedulerTaskExecutionContext(std::vector<LevelDescription*> const* levels, std::vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state, PerformanceCounters& perf_count);
	~PrioritySchedulerTaskExecutionContext();

	void join();

	procs_t get_id();

	static Self* get();

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams&& ... params);

	template<class CallTaskType, class Strategy, typename ... TaskParams>
		void spawn_prio(Strategy s, TaskParams&& ... params);

	boost::mt19937& get_rng();

	procs_t get_distance(Self* other);
	procs_t get_distance(Self* other, procs_t max_granularity_level);
	procs_t get_max_distance();
	procs_t get_max_distance(procs_t max_granularity_level);
	size_t get_current_finish_stack_depth();

private:
	void run();
	void execute_task(Task* task, StackElement* parent);
	void main_loop();
	void process_queue();
	bool process_queue_until_finished(StackElement* parent);
	void wait_for_finish(StackElement* parent);

	void empty_stack();
	StackElement* create_non_blocking_finish_region(StackElement* parent);
	void signal_task_completion(StackElement* stack_element);
	void finalize_stack_element(StackElement* element, StackElement* parent, size_t version, bool local);

	void start_finish_region();
	void end_finish_region();

	PerformanceCounters performance_counters;

	static size_t const stack_size;
	StackElement* stack;
	StackElement* current_task_parent;
	size_t stack_filled_left;
	size_t stack_filled_right;
	size_t stack_init_left;
//	size_t stack_init_right;
	// The use of the freed_stack_elements vector should lead to less usage of the stack by non-blocking tasks
	std::vector<StackElement*> freed_stack_elements;

	LevelDescription* levels;
	procs_t num_levels;

	CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold> > thread_executor;

	typename Scheduler::State* scheduler_state;

	size_t preferred_queue_length;
	size_t max_queue_length;
	bool call_mode;
	TaskStorage task_storage;

	boost::mt19937 rng;

	static thread_local Self* local_context;

	friend class CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>>;
	friend class Scheduler::Finish;
};

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
size_t const PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::stack_size = (8192 > (64 << CallThreshold))?8192:(64 << CallThreshold);

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
thread_local PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>*
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::local_context = NULL;

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::PrioritySchedulerTaskExecutionContext(std::vector<LevelDescription*> const* levels, std::vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state, PerformanceCounters& perf_count)
: performance_counters(perf_count), stack_filled_left(0), stack_filled_right(stack_size), stack_init_left(0)/*, stack_init_right(stack_size)*/, num_levels(levels->size()), thread_executor(cpus, this), scheduler_state(scheduler_state), preferred_queue_length(find_last_bit_set((*levels)[0]->total_size) << CallThreshold), max_queue_length(preferred_queue_length << 1), call_mode(false), task_storage(max_queue_length) {
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
		this->levels[i].memory_level = (*levels)[i]->memory_level;
	}
	for(size_t i = 0; i < num_levels; ++i) {
		this->levels[i].global_id_offset = local_id - this->levels[i].local_id;
	}

	performance_counters.finish_stack_blocking_min.add_value(stack_size);
	performance_counters.finish_stack_nonblocking_max.add_value(0);

	thread_executor.run();
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::~PrioritySchedulerTaskExecutionContext() {
	delete[] stack;
	delete[] levels;
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::join() {
	thread_executor.join();
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>*
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::get() {
	return local_context;
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
procs_t
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::get_id() {
	return levels[0].global_id;
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::run() {
	local_context = this;

	scheduler_state->state_barrier.wait(0, 1);

	Task* startup_task = scheduler_state->startup_task;
	if(startup_task != NULL) {
		if(PTR_CAS(&(scheduler_state->startup_task), startup_task, NULL)) {
			execute_task(startup_task, NULL);
		}
	}
	else {
		// Make sure the original initialization by all threads is visible
		MEMORY_FENCE();
	}
	main_loop();

	// We need to stop here, as it isn't safe after the barrier
	performance_counters.total_time.stop_timer();

	scheduler_state->state_barrier.barrier(1, levels[0].total_size);
	local_context = NULL;

	// Now we can safely finish execution
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::execute_task(Task* task, StackElement* parent) {
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

	// Check whether current_task_parent still is parent (if not, there is some error)
	assert(current_task_parent == parent);

	// Signal that we finished executing this task
	signal_task_completion(parent);
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::main_loop() {
	while(true) {
		// Make sure our queue is empty
		process_queue();

		{	// Local scope so we have a new backoff object
			Backoff bo;
			DequeItem di;
			performance_counters.idle_time.start_timer();
			while(true) {
				// Finalize elements in stack
				// We do not steal from the last level as there are no partners
				procs_t level = num_levels - 1;
				while(level > 0) {
					level--;
					// For all except the last level we assume num_partners > 0
					assert(levels[level].num_partners > 0);
					boost::uniform_int<procs_t> n_r_gen(0, levels[level].num_partners - 1);
					procs_t next_rand = n_r_gen(rng);
					assert(next_rand < levels[level].num_partners);
					assert(levels[level].partners[next_rand] != this);

					performance_counters.num_steal_calls.incr();
					performance_counters.steal_time.start_timer();
					typename Scheduler::StealerDescriptor sd(this, levels[level].partners[next_rand], num_levels - 1);
					di = levels[level].partners[next_rand]->task_storage.steal_push(this->task_storage, sd, performance_counters.task_storage_performance_counters);
				//	di = levels[level].partners[next_rand % levels[level].num_partners]->task_storage.steal();
					performance_counters.steal_time.stop_timer();

					if(di.task != NULL) {
						performance_counters.idle_time.stop_timer();
						execute_task(di.task, di.stack_element);
						delete di.task;
						break;
					}
					else {
						assert(task_storage.is_empty());
						performance_counters.num_unsuccessful_steal_calls.incr();
					}
				}
				if(di.task == NULL) {
					assert(task_storage.is_empty());
					if(scheduler_state->current_state >= 2) {
						performance_counters.idle_time.stop_timer();
						return;
					}
					task_storage.perform_maintenance(performance_counters.task_storage_performance_counters);
					bo.backoff();
				}
				else {
					break;
				}
			}
		}
	}
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::wait_for_finish(StackElement* parent) {
	while(parent->num_finished_remote + 1 != parent->num_spawned) {
		// Make sure our queue is empty
		if(!process_queue_until_finished(parent))
		{	// Local scope so we have a new backoff object
			Backoff bo;
			DequeItem di;
			while(true) {
				// Finalize elements in stack
				// We do not steal from the last level as there are no partners
				procs_t level = num_levels - 1;
				while(level > 0) {
					level--;
					// For all except the last level we assume num_partners > 0
					assert(levels[level].num_partners > 0);
					boost::uniform_int<procs_t> n_r_gen(0, levels[level].num_partners - 1);
					procs_t next_rand = n_r_gen(rng);
					assert(levels[level].partners[next_rand] != this);
					performance_counters.num_steal_calls.incr();
					typename Scheduler::StealerDescriptor sd(this, levels[level].partners[next_rand], num_levels - 1);
					di = levels[level].partners[next_rand]->task_storage.steal_push(this->task_storage, sd, performance_counters.task_storage_performance_counters);
				//	di = levels[level].partners[next_rand % levels[level].num_partners]->task_storage.steal();

					if(di.task != NULL) {
						execute_task(di.task, di.stack_element);
						delete di.task;
						break;
					}
					else {
						assert(task_storage.is_empty());
						performance_counters.num_unsuccessful_steal_calls.incr();
					}
				}
				if(di.task == NULL) {
					assert(task_storage.is_empty());
					if(parent->num_finished_remote + 1 == parent->num_spawned) {
						return;
					}
					task_storage.perform_maintenance(performance_counters.task_storage_performance_counters);
					bo.backoff();
				}
				else {
					break;
				}
			}
		}
	}
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::process_queue() {
	DequeItem di = task_storage.pop(performance_counters.task_storage_performance_counters);
	while(di.task != NULL) {
		// Warning, no distinction between locally spawned tasks and remote tasks
		// But this makes it easier with the finish construct, etc.
		// Otherwise we would have to empty our deque on the next finish call
		// which is bad for balancing
		execute_task(di.task, di.stack_element);
		delete di.task;
		di = task_storage.pop(performance_counters.task_storage_performance_counters);
	}
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
bool PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::process_queue_until_finished(StackElement* parent) {
	DequeItem di = task_storage.pop(performance_counters.task_storage_performance_counters);
	while(di.task != NULL) {
		// Warning, no distinction between locally spawned tasks and remote tasks
		// But this makes it easier with the finish construct, etc.
		// Otherwise we would have to empty our deque on the next finish call
		// which is bad for balancing
		execute_task(di.task, di.stack_element);
		delete di.task;
		if(parent->num_spawned == parent->num_finished_remote + 1) {
			return true;
		}
		di = task_storage.pop(performance_counters.task_storage_performance_counters);
	}
	return false;
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
typename PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::StackElement*
PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::create_non_blocking_finish_region(StackElement* parent) {
	if(freed_stack_elements.empty()) {
		// Perform cleanup on finish stack
		empty_stack();

		assert(stack_filled_left < stack_filled_right);

		if(stack_filled_left >= stack_init_left/* && stack_filled_left < stack_init_right*/) {
			stack[stack_filled_left].version = 0;
			++stack_init_left;
		}
		else {
			assert((stack[stack_filled_left].version & 1) == 1);
			++(stack[stack_filled_left].version);
		}
		assert((stack[stack_filled_left].version & 1) == 0);

		stack[stack_filled_left].num_finished_remote = 0;
		// As we create it out of a parent region that is waiting for completion of a single task, we can already add this one task here
		stack[stack_filled_left].num_spawned = 1;
		stack[stack_filled_left].parent = parent;

		++stack_filled_left;
		performance_counters.finish_stack_nonblocking_max.add_value(stack_filled_left);

		return &(stack[stack_filled_left - 1]);
	}
	else {
		StackElement* ret = freed_stack_elements.back();
		freed_stack_elements.pop_back();

		assert(ret->num_finished_remote == 0);
		assert((ret->version & 1) == 1);
		++(ret->version);
		assert((ret->version & 1) == 0);

		ret->num_spawned = 1;
		ret->parent = parent;

		return ret;
	}
}

/*
 * empty stack but not below limit
 */
template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::empty_stack() {
	assert(freed_stack_elements.empty());

	while(stack_filled_left > 0) {
		size_t se = stack_filled_left - 1;
		if(stack[se].num_spawned == stack[se].num_finished_remote
				&& (stack[se].version & 1)) {
	//		finalize_stack_element(&(stack[se]), stack[se].parent);

			stack_filled_left = se;

			// When parent is set to NULL, some thread is finalizing/has finalized this stack element (otherwise we would have an error)
			// Actually we have to check before whether parent has already been set to NULL, or we might have a race
		//	assert(stack[stack_filled_left].parent == NULL);
		}
		else {
			break;
		}
	}
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::signal_task_completion(StackElement* stack_element) {
	StackElement* parent = stack_element->parent;
	size_t version = stack_element->version;

	assert(stack_element->num_spawned > stack_element->num_finished_remote);

	bool local = stack_element >= stack && (stack_element < (stack + stack_size));
	if(local) {
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
		finalize_stack_element(stack_element, parent, version, local);
	}
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
inline void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::finalize_stack_element(StackElement* element, StackElement* parent, size_t version, bool local) {
	if(parent != NULL) {
		// We have to check if we are local too!
		// (otherwise the owner might already have modified element, and then num_spawned might be 0)
		// Rarely happens, but it happens!
		if(local && element->num_spawned == 0) {
			assert(element->num_finished_remote == 0);
			// No tasks processed remotely - no need for atomic ops
		//	element->parent = NULL;
			assert((element->version & 1) == 0);
			++(element->version);
			assert((element->version & 1) == 1);
			assert(element->version == version + 1);
			assert(element >= stack && (element < (stack + stack_size)));
			if((unsigned)(element - stack) + 1 == stack_filled_left) {
				--stack_filled_left;
			}
			else {
				freed_stack_elements.push_back(element);
			}
			signal_task_completion(parent);
		}
		else {
			assert((version & 1) == 0);
			if(PTR_CAS(&(element->version), version, version + 1)) {
				signal_task_completion(parent);
			}
		}
	}
	else {
		// Root element - we can shut down the scheduler
		scheduler_state->current_state = 2;
	}
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::start_finish_region() {
	performance_counters.task_time.stop_timer();
	performance_counters.num_finishes.incr();

	// Perform cleanup on left side of finish stack
	// Not allowed any more. may only be called if freed_stack_elements is empty
//	empty_stack();

	if(stack_filled_right < stack_size && stack[stack_filled_right].num_spawned == 1) {
		++(stack[stack_filled_right].reused_finish_depth);
	}
	else {
		assert(stack_filled_left < stack_filled_right);
		--stack_filled_right;
		performance_counters.finish_stack_blocking_min.add_value(stack_filled_right);

		stack[stack_filled_right].num_finished_remote = 0;
		// We add 1 to make sure finishes are not propagated to the parent (as we wait manually and then execute a continuation)
		stack[stack_filled_right].num_spawned = 1;
		stack[stack_filled_right].parent = current_task_parent;
		stack[stack_filled_right].reused_finish_depth = 0;

		// version not needed for finish, as finish blocks anyway
	/*
		if(stack_filled_right >= stack_init_left && stack_filled_right < stack_init_right) {
			stack[stack_filled_left].version = 0;
			--stack_init_right;
		}*/

		current_task_parent = stack + stack_filled_right;
	}
	performance_counters.task_time.start_timer();
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::end_finish_region() {
	performance_counters.task_time.stop_timer();

	// Make sure current_task_parent has the correct value
	assert(current_task_parent == &(stack[stack_filled_right]));

	if(current_task_parent->num_spawned > 1) {
		// There exist some non-executed or stolen tasks

		// Process other tasks until this task has been finished
		wait_for_finish(current_task_parent);

		// Repair pointer to current_task_parent (might be changed while waiting)
		current_task_parent = &(stack[stack_filled_right]);
	}
	if(current_task_parent->reused_finish_depth > 0) {
		--(current_task_parent->reused_finish_depth);
	}
	else {
		// Restore old parent
		current_task_parent = current_task_parent->parent;

		// Remove stack element
		++stack_filled_right;
	}
	performance_counters.task_time.start_timer();
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::finish(TaskParams&& ... params) {
	start_finish_region();

	call<CallTaskType>(static_cast<TaskParams&&>(params) ...);

	end_finish_region();
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::spawn(TaskParams&& ... params) {
	spawn_prio<CallTaskType>(DefaultStrategy<Scheduler>(), static_cast<TaskParams&&>(params) ...);
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
template<class CallTaskType, class Strategy, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::spawn_prio(Strategy s, TaskParams&& ... params) {
	performance_counters.num_spawns.incr();

	if(task_storage.is_full(performance_counters.task_storage_performance_counters)) {
		// Rigid limit in case the data-structure can not grow
		performance_counters.num_spawns_to_call.incr();
		call<CallTaskType>(static_cast<TaskParams&&>(params) ...);
	}
	else {
		// Limit dependent on currently not executed tasks of the current finish_stack element
		// to ensure repeated finish calls don't let the queues explode, each blocking finish call reduces the limit by 1
		size_t current_tasks = (current_task_parent->num_spawned - current_task_parent->num_finished_remote);
		size_t blocking_finish_depth = (stack_size - stack_filled_right);
		size_t limit = call_mode?preferred_queue_length:max_queue_length;
		if(blocking_finish_depth >= limit || current_tasks >= (limit - blocking_finish_depth)) {
			call_mode = true;
			performance_counters.num_spawns_to_call.incr();
			call<CallTaskType>(static_cast<TaskParams&&>(params) ...);
		}
		else {
			call_mode = false;
			performance_counters.num_actual_spawns.incr();
			CallTaskType* task = new CallTaskType(params ...);
			assert(current_task_parent != NULL);
			assert(current_task_parent >= stack && (current_task_parent < (stack + stack_size)));
			++(current_task_parent->num_spawned);
			DequeItem di;
			di.task = task;
			di.stack_element = current_task_parent;
			task_storage.push(s, di, performance_counters.task_storage_performance_counters);
		}
	}
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::call(TaskParams&& ... params) {
	performance_counters.num_calls.incr();
	// Create task
	CallTaskType task(static_cast<TaskParams&&>(params) ...);
	// Execute task
	task(*this);
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
boost::mt19937& PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::get_rng() {
	return rng;
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
procs_t PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::get_distance(Self* other) {
	return get_distance(other, num_levels - 1);
}


template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
procs_t PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::get_distance(Self* other, procs_t max_granularity_level) {
	assert(max_granularity_level < num_levels);

	procs_t offset = std::max(levels[max_granularity_level].memory_level, other->levels[max_granularity_level].memory_level);
	procs_t i = max_granularity_level;
	while(levels[i].global_id_offset != other->levels[i].global_id_offset) {
		assert(i > 0);
		--i;
	}
	assert(levels[i].memory_level <= offset);
	return offset - levels[i].memory_level;
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
inline procs_t PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::get_max_distance() {
	return this->levels[0].memory_level;
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
inline procs_t PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::get_max_distance(procs_t max_granularity_level) {
	assert(max_granularity_level < num_levels);

	return this->levels[max_granularity_level].memory_level;
}

template <class Scheduler, template <class Scheduler, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
inline procs_t PrioritySchedulerTaskExecutionContext<Scheduler, TaskStorageT, DefaultStrategy, CallThreshold>::get_current_finish_stack_depth() {
	return stack_size - stack_filled_right;
}

}

#endif /* PRIORITYSCHEDULERTASKEXECUTIONCONTEXT_H_ */
