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
#include <queue>
#include <assert.h>
#include <iostream>

/*
 *
 */
namespace pheet {

union BasicMixedModeSchedulerTaskExecutionContextRegistration {
	uint64_t complete;
	struct {
		uint32_t c;	// counter
		uint16_t r;	// required threads
		uint16_t a;	// acquired threads
	} parts;
};

struct BasicMixedModeSchedulerTaskExecutionContextFinishStackElement {
	// Modified by local thread. Incremented when task is spawned, decremented when finished
	size_t num_spawned;

	// Only modified by other threads. Stolen tasks that were finished (including spawned tasks)
	size_t num_finished_remote;

	// Pointer to num_finished_remote of another thread (the one we stole tasks from)
	BasicMixedModeSchedulerTaskExecutionContextFinishStackElement* parent;
};

/*
 * Information about the next task to execute
 *
 * after retrieving the TeamTaskData element, countdown has to be atomically decremented
 * the last thread to decrement countdown has to either:
 * - set the pointer on the team-stack to NULL, signaling, that all threads in the team have started execution, or
 * - delete the previous task and decrement num_finished_remote in the parent stack element
 *
 * As long as the next pointer is NULL, there is more to come. (Other threads just have to wait.
 * 	If out of sync they can execute other tasks in the meantime)
 * A TeamTaskData element with a task pointer set to NULL is the last in the series. There, countdown
 * 	also has to be decremented, to make sure the previous task is finished correctly
 */
//
template <class Task, class StackElement>
struct BasicMixedModeSchedulerTaskExecutionContextTeamTaskData {
	// Task to execute
	Task* task;

	// Pointer to the next task to execute. set by coordinator
	BasicMixedModeSchedulerTaskExecutionContextTeamTaskData<Task, StackElement>* next;

	// Team level of the following task - needed to ensure threads don't follow the next pointer if they are not needed
	// for memory reclamation
	procs_t next_team_level;

	// Parent stack element needed for signaling finish
	StackElement* parent;

	// Countdown for new threads not needed for the previous task
	// After the countdown is 0 we can assume all needed threads are in the team (but not necessarily in sync)
	procs_t newcomer_start_countdown;

	// Threads count down as soon as they finish executing the task.
	// The last thread to count down is responsible for cleanup of the task (signal finish, delete task)
	// (Warning: after the last countdown it is not guaranteed that the TeamData hasn't been reused,
	// so even the deleting thread should get the task beforehand)
	// TODO: How do we know when all threads have read the next pointer? There is no real signal for that
	procs_t executed_countdown;

	// Size of the team to execute the task
	procs_t team_size;

	// Level of the team to execute the task
	procs_t team_level;
};

struct BasicMixedModeSchedulerTaskExecutionContextTeamInfo {
	procs_t team_size;
	procs_t local_id;
	procs_t coordinator_id;
	procs_t team_level;

	// Only for non coordinating threads
	procs_t max_team_level;
};

template <class TaskExecutionContext>
struct BasicMixedModeSchedulerTaskExecutionContextLevelDescription {
	TaskExecutionContext** partners;
	procs_t num_partners;
	procs_t local_id;
	procs_t total_size;
	bool reverse_ids;
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
	typedef BasicMixedModeSchedulerTaskExecutionContextRegistration Registration;
	typedef BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> TaskExecutionContext;
	typedef BasicMixedModeSchedulerTaskExecutionContextLevelDescription<BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > LevelDescription;
	typedef typename Scheduler::Backoff Backoff;
	typedef typename Scheduler::CPUHierarchy CPUHierarchy;
	typedef typename Scheduler::Task Task;
	typedef BasicMixedModeSchedulerTaskExecutionContextFinishStackElement FinishStackElement;
	typedef BasicMixedModeSchedulerTaskExecutionContextDequeItem<BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > DequeItem;
	typedef BasicMixedModeSchedulerTaskExecutionContextTeamTaskData<Task, StackElement> TeamTaskData;
	typedef BasicMixedModeSchedulerTaskExecutionContextTeamInfo TeamInfo;

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

	// If team is out of sync
	void sync_team();
	void team_barrier();

	procs_t get_global_id();
	procs_t get_local_id();

private:
	void run();
	void execute_task(Task* task, StackElement* parent);
	void main_loop();
	void process_queue();
	void empty_stack(size_t limit);

	void build_team(procs_t nt_size);

	void visit_partners(procs_t level_limit);

	void follow_coordinator();

	procs_t get_level_for_num_threads(procs_t num_threads);

	// Stack is only used by the coordinator
	static size_t const stack_size;
	StackElement* stack;
	size_t stack_filled;

	// machine hierarchy
	LevelDescription* levels;
	procs_t num_levels;

	// Team task information
	TeamTaskData* last_team_task;
	TeamTaskData* current_team_task;
	TeamTaskData** announced_team_tasks;
	TaskExecutionContext** announced_coordinators;

	procs_t team_announcement_index;

	// Information needed for task reclamation scheme
	queue<TeamTaskData*> reclamation_queue;
	TeamTaskData* team_task_reuse;

	// Information whether current team is in sync (local)
	bool in_sync;

	// Information about the team (calculated by all threads)
	TeamInfo* team_info;

	CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > thread_executor;

	typename Scheduler::State* scheduler_state;

	StealingDeque<DequeItem> stealing_deque;

	friend class CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>>;
};

template <class Scheduler, template <typename T> class StealingDeque>
size_t const BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::stack_size = 256;

template <class Scheduler, template <typename T> class StealingDeque>
size_t const BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::team_stack_size = 256;

template <class Scheduler, template <typename T> class StealingDeque>
size_t const BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::local_team_stack_size = 256;

template <class Scheduler, template <typename T> class StealingDeque>
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::BasicMixedModeSchedulerTaskExecutionContext(vector<LevelDescription*> const* levels, vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state)
: stack_filled(0), num_levels(levels->size()), thread_executor(cpus, this), scheduler_state(scheduler_state), stealing_deque(find_last_bit_set((*levels)[0]->total_size - 1) << 4), team_announcement_index(0), last_executed_task_size(1), coordinated_finish_context(0) {
	stack = new StackElement[stack_size];
	this->levels = new LevelDescription[num_levels];
	procs_t local_id = 0;
	for(ptrdiff_t i = num_levels - 1; i >= 0; i--) {
		this->levels[i].partners = (*levels)[i]->partners;
		this->levels[i].num_partners = (*levels)[i]->num_partners;
		local_id += (*levels)[i]->local_id;
		this->levels[i].local_id = local_id;
		this->levels[i].total_size = (*levels)[i]->total_size;
		this->levels[i].reverse_ids = (*levels)[i]->reverse_ids;
	}

	announced_team_tasks = new TeamTaskData*[num_levels];
	announced_coordinators = new TaskExecutionContext*[num_levels];
	team_task_reuse = new TeamTaskData*[num_levels];
	for(procs_t i = 0; i < num_levels; ++i) {
		announced_team_tasks[i] = NULL;
		announced_coordinators[i] = NULL;

		team_task_reuse[i] = new TeamTaskData();
	}
	team_task_reuse[num_levels - 1]->team_size = 1;
	team_task_reuse[num_levels - 1]->executed_countdown = 1;
	team_task_reuse[num_levels - 1]->team_level = num_levels - 1;

	last_team_task = team_task_reuse[0];
	last_team_task->next = NULL;
	last_team_task->team_size = 1;
	last_team_task->executed_countdown = 0;

	current_task_data = NULL;

	thread_executor.run();
}

template <class Scheduler, template <typename T> class StealingDeque>
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::~BasicMixedModeSchedulerTaskExecutionContext() {
	delete[] stack;
	delete[] levels;
	delete[] announced_team_tasks;
	delete[] announced_coordinators;

	TeamTaskData* tmp;

	while((tmp = reclamation_queue.pop()) != NULL) {
		delete tmp;
	}

	for(procs_t i = 0; i < num_levels; ++i) {
		delete team_task_reuse[i];
	}
	delete[] team_task_reuse;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::join() {
	thread_executor.join();
}


template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::run() {
	scheduler_state->state_barrier.wait(0, 1);

	Task* startup_task = scheduler_state->startup_task;
	if(startup_task != NULL && PTR_CAS(&(scheduler_state->startup_task), startup_task, NULL)) {
		finish_task(scheduler_state->team_size, startup_task, NULL);

		// make sure there is no reordering by the compiler (is this really necessary?)
		MEMORY_FENCE();

		scheduler_state->current_state = 2; // finished
	}
	else {
		visit_partners_until_shutdown();
	}

	scheduler_state->state_barrier.barrier(1, levels[0].total_size);

	// Now we can safely finish execution
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish_task(size_t nt_size, Task* task, FinishStackElement* parent) {
	if(nt_size == 1) {
		if(last_team_task->team_size > 1) {
			announce_solo_task();
		}
		finish_announced_task(task, parent);
	}
	else {
		// Perform cleanup on finish stack
		empty_finish_stack();

		// Create new stack element for finish
		parent = start_finish_region(parent);

		// announce team
		TeamTaskData* tt = announce_team_task(nt_size, task, parent);

		// Execute task
		call_announced_task(task);

		// signal that we finished execution of the task
		signal_current_team_task_completion();

		// Send task to memory reclamation
		reclamation_queue.push(tt);

		// Do some other work until the stack element has been finished
		process_tasks_until_finished(parent);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call_task(size_t nt_size, Task* task, FinishStackElement* parent) {
	if(nt_size == 1) {
		if(last_team_task->team_size > 1) {
			announce_solo_task();
		}
		call_announced_task(task);
	}
	else {
		// announce team
		TeamTaskData* tt = announce_team_task(nt_size, task, parent);

		// Execute task
		call_announced_task(task);

		// signal that we finished execution of the task
		signal_current_team_task_completion();

		// Send task to memory reclamation
		reclamation_queue.push(tt);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_queue_task(size_t nt_size, Task* task, FinishStackElement* parent) {
	if(parent >= stack && (parent < (stack + stack_size))) {
		call_task(nt_size, task, parent);
	}
	else {
		finish_task(nt_size, task, parent);
	}
}


template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish_announced_task(Task* task, FinishStackElement* parent) {
	// Perform cleanup on finish stack
	empty_finish_stack();

	// Create new stack element for finish
	parent = start_finish_region(parent);

	// Execute task
	(*task)(*this);

	// Do some other work until the stack element has been finished
	process_tasks_until_finished(parent);
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call_announced_task(Task* task) {
	// Execute task
	(*task)(*this);

	// Bookkeeping - needed for correct cleanup etc.
	last_executed_task_size = team_size;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_announced_task(Task* task, FinishStackElement* parent) {
	if(parent >= stack && (parent < (stack + stack_size))) {
		call_announced_task(task);
	}
	else {
		finish_announced_task(task, parent);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
TeamTaskData* BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::announce_team_task(size_t nt_size, Task* task, FinishStackElement* parent) {
	TeamTaskData* team_task;

	assert(nt_size != 1);

	if(!reclamation_queue.empty() && reclamation_queue.front()->executed_countdown == 0) {
		TeamTaskData* tmp = reclamation_queue.pop();
		team_task = team_task_reuse[tmp->team_level];
		team_task_reuse[tmp->team_level] = tmp;
	}
	else {
		team_task = new TeamTaskData();
	}

	procs_t prev_max = max_team_size;

	// Calculates level etc. of the task
	prepare_coordinator_team_info(nt_size);

	team_task->team_level = team_level;
	team_task->team_size = nt_size;
	team_task->task = task;
	team_task->parent = parent;
	team_task->next = NULL;
	if(prev_max < max_team_size) {
		team_task->newcomer_start_countdown = max_team_size - prev_max;
	}
	else {
		team_task->newcomer_start_countdown = 0;
	}
	team_task->executed_countdown = max_team_size;

	last_team_task->next_team_level = team_level;

	// Memory fence needed to ensure everything has been written before we set the next pointer
	MEMORY_FENCE();

	last_team_task->next = team_task;

	return team_task;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::announce_solo_task() {
	// For solo tasks there is only a single team_task_data object we reuse all the time
	assert(last_team_task->team_size != 1);

	last_team_task->next_team_size = 1;

	// Memory fence needed to ensure everything has been written before we set the next pointer
	MEMORY_FENCE();

	last_team_task->next = team_task_reuse[num_levels - 1];

	// ids "berechnen", etc...
	prepare_coordinator_team_info(1);
/*
	team_size = 1;
	local_id = 0;
	coordinator_id = 0;
	team_level = num_levels - 1;
	max_team_size = 1;*/
}


template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::process_tasks_until_shutdown() {
	// pre-condition: queue must be empty
	assert(stealing_deque.is_empty());

	while(scheduler_state->current_state != 2) {
		// Try to join teams or steal tasks
		visit_partners();

		// Make sure our queue is empty
		process_queue();
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::process_tasks_until_finished(FinishStackElement* stack_element) {
	while(stack_element->num_finished_remote != stack_element->num_spawned) {
		// Empty our queue
		process_queue_until_finished(stack_element);

		if(stack_element->num_finished_remote == stack_element->num_spawned) {
			break;
		}

		// Try to join teams or steal tasks
		visit_partners_until_finished(stack_element);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::process_tasks_until_coordinator_resumes(TeamTaskData* parent_team_task) {
	process_queue();

	while(parent_team_task->next == NULL) {
		// Try to join teams or steal tasks
		visit_partners();

		// Make sure our queue is empty
		process_queue();
	}
}

/*
 * Stealing routine for idle threads
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::visit_partners() {
	Backoff bo;
	DequeItem di;
	while(true) {
		procs_t next_rand = random();

		// We do not steal from the last level as there are no partners
		procs_t level = num_levels - 1;
		while(level > 0) {
			--level;
			// For all except the last level we assume num_partners > 0
			assert(levels[level].num_partners > 0);
			ThreadExecutionContext* partner = levels[level].partners[next_rand % levels[level].num_partners];
			assert(partner != this);

			ThreadExecutionContext* coordinator = find_partner_team(partner, level);
			if(coordinator != NULL) {
				join_coordinator(coordinator);
				return;
			}

			di = steal_tasks_from_partner(partner, level);
		//	di = levels[level].partners[next_rand % levels[level].num_partners]->stealing_deque.steal();

			if(di.task != NULL) {
				execute_queue_task(di.team_size, di.task, di.stack_element);
				return;
			}
		}
		if(scheduler_state->current_state >= 2) {
			return;
		}
		bo.backoff();
	}
}

/*
 * Stealing routine for threads waiting for finish
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::visit_partners_until_finished(FinishStackElement* stack_element) {
	Backoff bo;
	DequeItem di;
	while(stack_element->num_finished_remote != stack_element->num_spawned) {
		procs_t next_rand = random();

		// We do not steal from the last level as there are no partners
		procs_t level = num_levels - 1;
		while(level > 0) {
			--level;
			// For all except the last level we assume num_partners > 0
			assert(levels[level].num_partners > 0);
			ThreadExecutionContext* partner = levels[level].partners[next_rand % levels[level].num_partners];
			assert(partner != this);

			ThreadExecutionContext* coordinator = find_partner_team(partner, level);
			if(coordinator != NULL) {
				join_coordinator(coordinator);
				return;
			}

			di = steal_tasks_from_partner(partner, level);
		//	di = levels[level].partners[next_rand % levels[level].num_partners]->stealing_deque.steal();

			if(di.task != NULL) {
				execute_queue_task(di.team_size, di.task, di.stack_element);
				return;
			}
		}
		if(scheduler_state->current_state >= 2) {
			return;
		}
		bo.backoff();
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
FinishStackElement* BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::start_finish_region(FinishStackElement* parent) {
	assert(stack_filled < stack_size);

	stack[stack_filled].num_finished_remote = 0;
	stack[stack_filled].num_spawned = 1;
	stack[stack_filled].parent = parent;

	++stack_filled;

	return &(stack[stack_filled - 1]);
}

/*
 * empty stack but not below limit
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::empty_finish_stack() {
	while(stack_filled > 0) {
		size_t se = stack_filled - 1;
		if(stack[se].num_spawned == stack[se].num_finished_remote) {
			finalize_finish_stack_element(se);

			stack_filled = se;
			stack[stack_filled].state_counter++;

			assert(stack[stack_filled].parent == NULL);
		}
		else {
			break;
		}
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::signal_current_team_task_completion() {
	if((PROCST_FETCH_AND_SUB(&(current_task_data->countdown), 1)) == 1) {
		// Last thread to finish execution - do some cleanup

		assert(current_task->parent != NULL);

		// Note: coordinator will never do this as it always executes all tasks first so it can never be last
		// (except for single-threaded tasks)
		signal_task_completion(current_task->parent);

		// For teams we have to always use dynamic memory allocation anyway, so we can always delete the task
		delete current_task_data->task;
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::signal_task_completion(FinishStackElement* stack_element) {
	FinishStackElement* parent = stack_element->parent;

	if(stack_element >= stack && (stack_element < (stack + stack_size))) {
		--(stack_element->num_spawned);

		// Memory fence is unfortunately required to guarantee that some thread finalizes the finish_stack_element
		// TODO: prove that the fence is enough
		MEMORY_FENCE();
	}
	else {
		// Increment num_finished_remote of parent
		SIZET_ATOMIC_ADD(&(stack_element->num_finished_remote), 1);
	}
	if(stack_element->num_spawned == stack_element->num_finished_remote) {
		finalize_finish_stack_element(stack_element, parent);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finalize_finish_stack_element(FinishStackElement* element, FinishStackElement* parent) {
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
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::process_queue() {
	DequeItem di = get_next_local_task();
	while(di.task != NULL) {
		execute_queue_task(di.team_size, di.task, di.stack_element);

	//	empty_stack(0);
		di = get_next_local_task();
	}
}


template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::process_queue_until_finished(FinishStackElement* stack_element) {
	DequeItem di = get_next_local_task();
	while(di.task != NULL) {
		execute_queue_task(di.team_size, di.task, di.stack_element);

		if(stack_element->num_finished_remote == stack_element->num_spawned) {
			break;
		}

	//	empty_stack(0);
		di = get_next_local_task();
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::join_coordinator(TaskExecutionContext* coor) {
	// Store previous coordinator
	TaskExecutionContext* prev_coor = announced_coordinators[team_announcement_index];
	announced_coordinators[team_announcement_index] = coor;

/*	procs_t team_level = team->team_level;
	assert(team_level < (num_levels - 1));
	procs_t sub_team_size = coordinator->levels[team_level + 1].total_size;
	procs_t max_team_size = levels[team_level].total_size;
	procs_t local_id = levels[team_level].local_id;
	procs_t coordinator_id = coordinator->levels[team_level].local_id;
	procs_t team_size = current_task_data->team_size;
	if(coordinator_id >= (max_team_size - sub_team_size)) {
		// Reverse ids if coordinator is in the right sub-team
		coordinator_id = max_team_size - coordinator_id - 1;
		local_id = max_team_size - local_id - 1;
	}*/

	TeamInfo ti;
	team_info = &ti;

	current_task_data = coor->announced_team_tasks[team_announcement_index];
	prepare_team_info(coor, current_task_data->team_level, current_task_data->team_size);

	// Calculate the team_level at which we have to drop out
	{
		assert(levels[ti.team_level].local_id != coor->levels[ti.team_level].local_id);

		TaskExecutionContext* smaller;
		TaskExecutionContext* larger;
		if(levels[ti.team_level].local_id < coor->levels[ti.team_level].local_id) {
			smaller = this;
			larger = coor;
		}
		else {
			smaller = coor;
			larger = this;
		}
		procs_t diff = larger->levels[ti.team_level].local_id - smaller->levels[ti.team_level].local_id;
		procs_t lvl = ti.team_level + 1;
		while(smaller->levels[ti.team_level].local_id + diff == larger->levels[ti.team_level].local_id) {
			++lvl;
		}
		ti.max_team_level = lvl - 1;
	}

	// First task should always exist
	assert(current_task_data->task != NULL);

	procs_t ctd = PROCST_FETCH_AND_SUB(&(current_task_data->newcomer_start_countdown), 1);
	// It should never happen that we have more newcomers than relevant
	assert(ctd != 0);
	if(ctd == 1) {
		// Last thread to find first task - do some cleanup
		coor->announced_team_tasks[team_announcement_index] = NULL;
	}

	while(true) {
		// Need to do this before
		if(ti.team_size <= ti.local_id) {
			// Thread not needed for next task, but still in team
			in_sync = false;
		}
		else {
			// Execute task
			call_announced_task(current_task_data->task);
		}
		// signal that we finished execution of the task
		signal_current_team_task_completion();

		// Wait for next task to be ready
		Backoff bo;
		if(in_sync) {
			while(current_task_data->next == NULL) {
				// make sure we stay in sync, so only back off
				bo.backoff();
			}
		}
		else {
			// We are out of sync. Do something different until we proceed
			if(current_task_data->next == NULL) {
				process_tasks_until_coordinator_resumes(current_task_data);
			}
		}

		// Signal that we have moved on to next task
		TeamTaskData* next_task = current_task_data->next;
		procs_t next_team_level = current_task_data->next_team_level;

		if(next_team_level > max_team_level) {
			// This thread isn't needed for the team any more
			break;
		}
		if(next_task->team_size > ti.team_size) {
			in_sync = false;
		}

		current_task_data = next_task;

/*		// Update team_size etc...
		assert(current_task_data->team_size <= max_team_size);
		if(team_size < next_task->team_size) {
			in_sync = false;
		}

		team_size = current_task_data->team_size;
		if(team_size <= sub_team_size) {
			do{
				// Team is resized to a smaller team
				++team_level;
				sub_team_size = coordinator->levels[team_level + 1].total_size;
			}while(team_size <= sub_team_size);

			max_team_size = coordinator->levels[team_level].total_size;

			local_id = levels[team_level].local_id;
			coordinator_id = coordinator->levels[team_level].local_id;

			if(coordinator_id >= (max_team_size - sub_team_size)) {
				// Reverse ids if coordinator is in the right sub-team
				coordinator_id = max_team_size - coordinator_id - 1;
				local_id = max_team_size - local_id - 1;
			}

			if(local_id >= sub_team_size) {
				// Thread not needed for team any more as team has been resized
				break;
			}
		}*/

		prepare_team_info(coor, next_team_level, current_task_data->team_size);
	}

	// Make sure we don't exit coordination after team_announcement has been set to NULL
	// (or we might end up reexecuting the task)
	// TODO: Make sure either all or none of the threads enter here?
	if(coor->announced_team_tasks[team_announcement_index] != NULL) {
		wait_for_team_joins();
	}

	// Restore previous coordinator (Needed e.g. when another team has been joined for tie-breaking
	announced_coordinators[team_announcement_index] = prev_coor;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::follow_coordinator_finish() {
	TeamInfo* old_team_info = team_info;
	TeamInfo ti = *old_team_info;
	team_info = &ti;

	{
		// Wait for next task to be ready
		Backoff bo;
		if(in_sync) {
			while(current_task_data->next == NULL) {
				// make sure we stay in sync, so only back off
				bo.backoff();
			}
		}
		else {
			// We are out of sync. Do something different until we proceed
			if(current_task_data->next == NULL) {
				process_tasks_until_coordinator_resumes(current_task_data);
			}
		}
	}

	while(true) {
		Task* task = current_task_data->task;

		// Need to do this before
		FinishStackElement* parent = current_task->parent;
		if(team_size <= local_id) {
			// Thread not needed for next task, but still in team
			in_sync = false;
		}
		else {
			// Execute task
			call_announced_task(task);
		}
		// current_task_data might change after execution, so make sure you have read anything you need beforehand

		// Wait for next task to be ready
		Backoff bo;
		if(in_sync) {
			while(current_task_data->next == NULL) {
				// make sure we stay in sync, so only back off
				bo.backoff();
			}
		}
		else {
			// We are out of sync. Do something different until we proceed
			if(current_task_data->next == NULL) {
				process_tasks_until_coordinator_resumes(current_task_data);
			}
		}

		// Signal that we have moved on to next task
		TeamTaskData* next_task = current_task_data->next;
		if((PROCST_FETCH_AND_SUB(&(next_task->countdown), 1)) == 1) {
			// Last thread to find next task - do some cleanup
			// TeamTaskData might already have been reused at this point, so don't access it any more!

			assert(parent != NULL);

			// Note: coordinator will never do this as it always executes all tasks first so it can never be last
			// (except for single-threaded tasks)
			signal_task_completion(parent);

			// For teams we have to always use dynamic memory allocation anyway, so we can always delete the task
			delete task;
		}
		current_task_data = next_task;

/*		// Update team_size etc...
		assert(current_task_data->team_size <= max_team_size);
		if(team_size < next_task->team_size) {
			in_sync = false;
		}

		team_size = current_task_data->team_size;
		if(team_size <= sub_team_size) {
			do{
				// Team is resized to a smaller team
				++team_level;
				sub_team_size = coordinator->levels[team_level + 1].total_size;
			}while(team_size <= sub_team_size);

			max_team_size = coordinator->levels[team_level].total_size;

			local_id = levels[team_level].local_id;
			coordinator_id = coordinator->levels[team_level].local_id;

			if(coordinator_id >= (max_team_size - sub_team_size)) {
				// Reverse ids if coordinator is in the right sub-team
				coordinator_id = max_team_size - coordinator_id - 1;
				local_id = max_team_size - local_id - 1;
			}

			if(local_id >= sub_team_size) {
				// Thread not needed for team any more as team has been resized
				break;
			}
		}*/
		if(current_task_data->task == NULL) {
			// Used to signal finish
			break;
		}

		if(!prepare_team_info(coor)) {
			// If this thread is not needed for execution anymore
			// TODO: make sure we notice a finish
			break;
		}
	}

	team_info = old_team_info;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::prepare_team_info(TaskExecutionContext* coor, procs_t level, procs_t size) {
	team_info->team_level = level;
	team_info->team_size = size;
	if(coor->levels[level].reverse_ids) {
		procs_t offset = coor->levels[level].total_size - 1;
		team_info->coordinator_id = max - coor->levels[level].local_id;
		team_info->local_id = max - levels[level].local_id;
	}
	else {
		team_info->coordinator_id = coor->levels[level].local_id;
		team_info->local_id = levels[level].local_id;
	}
}

/*
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::coordinate_team_until_finished(FinishStackElement* to_finish) {
	// Do some other work in the team until the stack element has been finished
	// No stealing! No execution of tasks > team level.
	// Processing of unrelated tasks may postpone continuation of parent task.
	// When either queue is empty, or stack element is finished, this method terminates

	// calculate ids, etc.
	procs_t team_level = team->team_level;
	assert(team_level < (num_levels - 1));
	procs_t sub_team_size = levels[team_level + 1].total_size;
	procs_t max_team_size = levels[team_level].total_size;
	procs_t coordinator_id = levels[team_level].local_id;
	procs_t team_size;
	if(coordinator_id >= (max_team_size - sub_team_size)) {
		// Reverse ids if coordinator is in the right sub-team
		coordinator_id = max_team_size - coordinator_id - 1;
	}
	procs_t local_id = coordinator_id;

	while(to_finish->num_spawned != to_finish_num_finished_remote) {
		// Get new queue item (only returns tasks with np > 1 and level >= team_level)
		DequeItem di = get_next_local_team_task(team_level);
		if(di == NULL) {
			// out of work. we can exit
			break;
		}

		TeamTaskData* task_data = new TeamTaskData();
		task_data->team_size = di.team_size;
		task_data->free_memory = true;
		task_data->countdown = max_team_size - 1;
		task_data->next = NULL;
		task_data->parent = di.stack_element;
		task_data->task = di.task;

		// Make sure all changes to task_data are visible before the pointer is available
		MEMORY_FENCE();
		current_task_data->next = task_data;

		// Check if team has been resized
		if(team_size <= sub_team_size) {
			do{
				// Team is resized to a smaller team
				++team_level;
				sub_team_size = levels[team_level + 1].total_size;
			}while(team_size <= sub_team_size);
			max_team_size = levels[team_level].total_size;

			coordinator_id = levels[team_level].local_id;

			if(coordinator_id >= (max_team_size - sub_team_size)) {
				// Reverse ids if coordinator is in the right sub-team
				coordinator_id = max_team_size - coordinator_id - 1;
			}
			local_id = coordinator_id;
		}

		execute_team_task(di.task, di.stack_element);
	}

	// Signal exit to others
	TeamTaskData* task_data = new TeamTaskData();
	task_data->team_size = max_team_size;
	task_data->free_memory = true;
	task_data->countdown = max_team_size - 1;
	task_data->next = NULL;
	task_data->parent = NULL;
	task_data->task = NULL;

	MEMORY_FENCE();
	current_task_data->next = task_data;
	current_task_data = task_data;
}
*/

/*
 * Makes sure all threads of the team are executing this task after the call
 *
 * No barrier semantics if all threads are already executing this task!
 * (If you need barrier semantics, use team_barrier instead)
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::sync_team() {
	if(!in_sync) {
		SyncStackElement* sync = &(sync_stack[coordinator_sync_stack_filled-1]);
		Registration old_reg;
		old_reg.complete = sync->reg.complete;
		uint32_t c = old_reg.parts.c;


		if((c & 1) == 0) {
			// First thread to see uninitialized stack element has to initialize it
			if(this->is_coordinator && coordinator_sync_stack_filled == (sync_stack_initialized - 1)) {
				sync_stack[sync_stack_initialized].reg.parts.a = 0;
				sync_stack[sync_stack_initialized].reg.parts.r = 0;
				sync_stack[sync_stack_initialized].reg.parts.c = 0;
			}

			++c;

			Registration reg;
			reg.parts.c = c;
			reg.parts.a = 1;
			reg.parts.r = team_size;
			if(!UINT64_CAS(&(sync->reg.complete), old_reg.complete, reg.complete)) {
				// Another thread was faster. just register
				register_for_team(sync);
			}
		}
		else {
			// Try registering
			Registration reg;
			reg.complete = old_reg.complete;
			++reg.parts.a;
			if(reg.parts.a == reg.parts.r) {
				++reg.parts.c;
			}
			if(!UINT64_CAS(&(sync->reg.complete), old_reg.complete, reg.complete)) {
				// Another thread was faster. just register
				register_for_team(sync);
			}
		}

		++coordinator_sync_stack_filled;

		build_team(&(sync_stack[sync_stack_filled-1]), c);

		--coordinator_sync_stack_filled;
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::build_team(SyncStackElement* sync, uint32_t c) {
	Registration reg;
	Backoff bo;
	reg.complete = sync->reg.complete;
	procs_t level = local_team_info->team_level;
	while(reg.parts.c == c && reg.parts.r != reg.parts.a) {
		if(!process_queue_task(level + 1, sync, c)) {
			if(!visit_team_partners(level, sync, c)) {
				bo.backoff();
			}
		}

		reg.complete = sync.reg.complete;
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::process_queue_task(procs_t level_limit, SyncStackElement* sync, uint32_t c) {
	if(has_local_task(level_limit)) {
		if(!deregister_from_team(sync, c)) {
			return true;
		}

		DequeItem di = get_next_local_task(level_limit);
		if(di.task != NULL) {
			// Warning, no distinction between locally spawned tasks and remote tasks
			// But this makes it easier with the finish construct, etc.
			// Otherwise we would have to empty our deque on the next finish call
			// which is bad for balancing
			execute_queue_task(di.team_size, di.task, di.stack_element);

			register_for_team(sync);
			return true;
		}
		register_for_team(sync);
	}
	return false;
}

template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::visit_team_partners(procs_t level_limit, SyncStackElement* sync, uint32_t c) {
	procs_t next_rand = random();

	// We do not steal from the last level as there are no partners
	procs_t level = num_levels - 1;
	while(level > level_limit) {
		--level;
		// For all except the last level we assume num_partners > 0
		assert(levels[level].num_partners > 0);
		ThreadExecutionContext* partner = levels[level].partners[next_rand % levels[level].num_partners];
		assert(partner != this);

		TeamInfo* team = find_partner_team(partner, level);
		if(team != NULL) {
			if(!deregister_from_team(sync, c)) {
				return true;
			}
			join_partner_team(team);
			follow_coordinator();

			register_for_team(sync);
			return true;
		}

		if(partner->has_local_task(level_limit)) {
			if(!deregister_from_team(sync, c)) {
				return true;
			}

			di = steal_tasks_from_partner(partner, level_limit);
		//	di = levels[level].partners[next_rand % levels[level].num_partners]->stealing_deque.steal();

			if(di.task != NULL) {
				execute_queue_task(di.team_size, di.task, di.stack_element);

				register_for_team(sync);
				return true;
			}

			register_for_team(sync);
		}
	}
	return false;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::register_for_team(SyncStackElement* sync) {
	// We can assume that we only get to register for teams we are definitely eligible for and that those teams are still
	// being built (because without this thread the team can't be built, and team building is never cancelled)
	Registration reg;
	Registration old_reg;
	Backoff bo;

	reg.complete = sync->reg.complete;
	old_reg.complete = reg.complete;

	++reg.parts.a;
	if(reg.parts.a == reg.parts.r) {
		++reg.parts.c;
	}

	while(!UINT64_CAS(&(sync->reg.complete), old_reg.complete, reg.complete)) {
		bo.backoff();

		reg.complete = sync->reg.complete;
		old_reg.complete = reg.complete;

		++reg.parts.a;

		if(reg.parts.a == reg.parts.r) {
			++reg.parts.c;
		}
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::deregister_from_team(SyncStackElement* sync, uint32_t c) {
	// If c hasn't changed and team isn't jetzt built we can deregister

	Registration reg;
	Registration old_reg;
	Backoff bo;

	reg.complete = sync->reg.complete;
	if(reg.parts.c != c) {
		// Team has already been built
		return false;
	}
	old_reg.complete = reg.complete;

	--reg.parts.a;

	while(!UINT64_CAS(&(sync->reg.complete), old_reg.complete, reg.complete)) {
		bo.backoff();

		reg.complete = sync->reg.complete;
		if(reg.parts.c != c) {
			// Team has already been built
			return false;
		}
		old_reg.complete = reg.complete;

		--reg.parts.a;
	}
	return true;
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish(TaskParams ... params) {
	if(is_coordinator()) {
		assert(stack_filled > 0);

		if(team_size == 1) {
			// Create task
			CallTaskType task(params ...);

			// Create a new stack element for new task
			// num_finished_remote is not required as this stack element blocks lower ones from finishing anyway
			finish_team_task(&task, NULL);
		}
		else {
			CallTaskType* task = new CallTaskType(params ...);

			// Create a new stack element for new task
			// num_finished_remote is not required as this stack element blocks lower ones from finishing anyway
			finish_task(team_size, task, NULL);
		}
	}
	else {
		join_coordinator_subteam();
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::spawn(TaskParams ... params) {
	// TODO: optimization to use call in some cases to prevent the stack from growing too large

	// TODO: let tasks be spawned by multiple threads

	if(is_coordinator()) {
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
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call(TaskParams ... params) {
	if(is_coordinator) {
		assert(stack_filled > 0);

		if(team_size == 1) {
			CallTaskType task(params ...);
			call_team_task(&task);
		}
		else {
			CallTaskType* task = new CallTaskType(params ...);
			call_task(team_size, task, &(stack[stack_filled - 1]));
		}
	}
	else {
		join_coordinator_subteam();
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish_nt(procs_t nt_size, TaskParams ... params) {
	if(is_coordinator()) {
		assert(stack_filled > 0);

		if(nt_size == 1) {
			// Create task
			CallTaskType task(params ...);

			// Create a new stack element for new task
			// num_finished_remote is not required as this stack element blocks lower ones from finishing anyway
			finish_team_task(&task, NULL);
		}
		else {
			CallTaskType* task = new CallTaskType(params ...);

			// Create a new stack element for new task
			// num_finished_remote is not required as this stack element blocks lower ones from finishing anyway
			finish_task(nt_size, task, NULL);
		}
	}
	else {
		join_coordinator_subteam(nt_size);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::spawn_nt(procs_t nt_size, TaskParams ... params) {
	// TODO: optimization to use call in some cases to prevent the stack from growing too large

	// TODO: let tasks be spawned by multiple threads

	if(is_coordinator()) {
		CallTaskType* task = new CallTaskType(params ...);
		assert(stack_filled > 0);
		stack[stack_filled - 1].num_spawned++;
		DequeItem di;
		di.task = task;
		di.stack_element = &(stack[stack_filled - 1]);
		di.team_size = nt_size;
		stealing_deque.push(di);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call_nt(procs_t nt_size, TaskParams ... params) {
	if(is_coordinator()) {
		assert(stack_filled > 0);

		if(team_size == 1) {
			CallTaskType task(params ...);
			call_team_task(&task);
		}
		else {
			CallTaskType* task = new CallTaskType(params ...);
			call_task(team_size, task, &(stack[stack_filled - 1]));
		}
	}
	else {
		join_coordinator_subteam(nt_size);
	}
}



















template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_task(Task* task, StackElement* parent) {
	assert(stack_filled < stack_size);

	stack[stack_filled].num_finished_remote = 0;
	stack[stack_filled].num_spawned = 0;
	stack[stack_filled].parent = parent;

	++stack_filled;

	if(nt_size > 1) {
		++stack[stack_filled].num_spawned;

		TeamTaskData* next_task = new TeamTaskData();
		next_task->task = task;
		next_task->parent = &(stack[stack_filled-1]);
		next_task->countdown = nt_size - 1;
		next_task->team_size = nt_size;
		next_task->next = NULL;
		next_task->stack_height = stack_filled;
		// TODO: decide about free_memory
		MEMORY_FENCE();

		if(team_announcement != NULL) {
			team_announcement->task = next_task;
		}
		else {
			// Needed to check termination of previous task
			next_task->countdown = team_size - 1;
			current_task_data->next = next_task;
		}
		current_task_data = next_task;

		(*task)(*this);
	}
	else {
		(*task)(*this);
		delete task;
	}
}
















/**
 * translate a number of threads to a level in the CPU hierarchy
 */
template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_level_for_num_threads(procs_t num_threads) {
	procs_t candidate = num_levels;
	while(candidate > 0 && levels[candidate - 1].total_size < num_threads) {
		--candidate;
	}
/*	procs_t candidate = find_last_bit_set(num_threads);
	while(candidate < num_levels && levels[candidate].total_size < num_threads) {
		++candidate;
	}*/
	return candidate - 1;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::follow_coordinator(RemoteTeamAnnouncement* team) {
//	TeamTaskData* current_task_data;
	Task* task;

	TeamInfo team_info;

	team_info.coordinator = team->coordinator;

	team_info.team_level = team->team_level;
	assert(team_info.team_level < (num_levels - 1));
	team_info.sub_team_size = levels[team_info.team_level + 1].total_size;
	team_info.max_team_size = levels[team_info.team_level].total_size;
	team_info.local_id = levels[team_info.team_level].local_id;
	team_info.coordinator_id = team_info.coordinator->levels[team_level].local_id;
	if(team_info.coordinator_id >= team_info.sub_team_size) {
		// Reverse ids if coordinator is in the right sub-team
		team_info.coordinator_id = team_info.max_team_size - team_info.coordinator_id - 1;
		team_info.local_id = team_info.max_team_size - team_info.local_id - 1;
		team_info.sub_team_size = team_info.max_team_size - team_info.sub_team_size;
	}

	{
		TeamAnnouncement* team_data = team->team_data;
		Backoff bo;
		while(team_data->task_data == NULL) {
			bo.backoff();
		}
		current_task_data = team_data->task_data;

		task = current_task_data->task;
		if((PROCST_FETCH_AND_SUB(&(current_task_data->countdown), 1)) == 1) {
			// Last task to follow coordinator - do some cleanup
			team_data->task_data = NULL;

			// TODO: do we need to remove the team announcement as well?
		}
	}

	while(task != NULL) {
		team_info.active = true;

		// Needed for team shrinking
		// Member var team_size (accessible to task)
		team_info.team_size = current_task_data->team_size;
		while(team_info.team_size <= team_info.sub_team_size) {
			if(team_info.local_id >= team_info.sub_team_size) {
				// Thread not needed for team any more
				return;
			}
			team_info.team_level++;
			team_info.max_team_size = team_info.sub_team_size;
			team_info.sub_team_size = levels[team_level + 1].total_size;
			team_info.local_id = levels[team_info.team_level].local_id;
			team_info.coordinator_id = coordinator->levels[team_info.team_level].local_id;

			if(team_info.coordinator_id >= team_info.sub_team_size) {
				// Reverse ids if coordinator is in the right sub-team
				team_info.coordinator_id = team_info.max_team_size - team_info.coordinator_id - 1;
				team_info.local_id = team_info.max_team_size - team_info.local_id - 1;
				team_info.sub_team_size = team_info.max_team_size - team_info.sub_team_size;
			}
		}
		if(team_info.team_size >= team_info.local_id) {
			// Thread not needed for team any more
			return;
		}

		local_team_info = &team_info;

		// Execute task
		(*(current_task_data->task))(*this);

		{// New environment to get a new backoff object
			Backoff bo;
			TeamTaskData* next_task;
			if(in_sync) {
				while((next_task = current_task_data->next) == NULL) {
					// Spin until next task is available
					bo.backoff();
				}
			}
			else {
				while((next_task = current_task_data->next) == NULL) {
					if(!process_queue_task(team_info.team_level + 1)) {
						if(!visit_partners(team_info.team_level)) {
							bo.backoff();
						}
					}
				}
			}
			team_info.active = false;
			if((PROCST_FETCH_AND_SUB(&(next_task->countdown), 1)) == 1) {
				// Last thread to find next task - do some cleanup

				StackElement* parent = current_task->parent;
				if(parent != NULL) {
					// Note: coordinator will never do this as it always executes all tasks first so it can never be last
					// (except for single-threaded tasks)

					if(parent >= stack && (parent < (stack + stack_size))) {
						// Parent is actually local. No need for atomics
						parent->num_spawned--;
					}
					else {
						// Increment num_finished_remote of parent
						SIZET_ATOMIC_ADD(&(parent->num_finished_remote), 1);
					}
				}
				if(current_task_data->free_memory) {
					delete current_task_data->task;
					delete current_task_data;
				}
			}
			current_task_data = next_task;
			task = current_task_data->task;
		}
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::is_coordinator() {
	return coordinator_id == local_id;
}

template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_local_id() {
	return local_team_info->local_id;
}

template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_coordinator_id() {
	return local_team_info->coordinator_id;
}

template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_global_id() {
	return levels[0].local_id;
}


template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish_nt(procs_t nt_size, TaskParams ... params) {
	if(nt_size == team_size) {
		finish(params ...);
	}
	else if(nt_size > team_size) {
		if(current_local_team->coordinator == this) {
			// TODO: Create new entry on team stack
		}
		// Just go to the next entry on the team stack
		LocalTeamStackElement* next_local_team = current_local_team + 1;
		next_local_team->coordinator = current_local_team->coordinator;
		next_local_team->team_stack_element = current_local_team->team_stack_element + 1;

		// TODO: check whether we need a memory fence or if this is safe
		current_local_team = next_local_team;

		// Create new environment
		in_sync = false;

		procs_t old_team_size = team_size;
		team_size = nt_size;

		// TODO: build team

		// execute task in current team
		finish(params ...);

		// Revert team size
		team_size = old_team_size;

		// in_sync  stays the same as subteam was larger

		// Go back in stack
		--current_local_team;
	}
	else {
		if(local_id >= nt_size) {
			// drop out of team and do something different until execution is finished
		}
		else {
			// Create new environment
			procs_t old_team_size = team_size;
			team_size = nt_size;

			// execute task in current team
			finish(params ...);

			// Revert team size
			team_size = old_team_size;

			// team is now out of sync
			in_sync = false;
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
		// TODO: let tasks be spawned by multiple threads not only by coor

		if(current_local_team->coordinator == this) {
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
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call_nt(procs_t nt_size, TaskParams ... params) {
	if(nt_size == team_size) {
		call(params ...);
	}
	else if(nt_size > team_size) {
		if(current_local_team->coordinator == this) {
			// TODO: Create new entry on team stack
		}
		// Just go to the next entry on the team stack
		LocalTeamStackElement* next_local_team = current_local_team + 1;
		next_local_team->coordinator = current_local_team->coordinator;
		next_local_team->team_stack_element = current_local_team->team_stack_element + 1;

		// TODO: check whether we need a memory fence or if this is safe
		current_local_team = next_local_team;

		// Create new environment
		in_sync = false;

		procs_t old_team_size = team_size;
		team_size = nt_size;

		// TODO: build team

		// execute task in current team
		call(params ...);

		// Revert team size
		team_size = old_team_size;

		// in_sync  stays the same as subteam was larger

		// Go back in stack
		--current_local_team;
	}
	else {
		if(local_id >= nt_size) {
			// drop out of team and do something different until call is finished
		}
		else {
			// Create new environment
			procs_t old_team_size = team_size;
			team_size = nt_size;

			// execute task in current team
			call(params ...);

			// Revert team size
			team_size = old_team_size;

			// team is now out of sync
			in_sync = false;
		}
	}
}


}

#endif /* BASICMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_ */
