/*
 * BasicMixedModeSchedulerTaskExecutionContext.h
 *
 *  Created on: 16.06.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef BASICMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_
#define BASICMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_

#include "../../../settings.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../common/FinishRegion.h"
#include "../../../misc/atomics.h"
#include "../../../misc/bitops.h"
#include "../../../misc/type_traits.h"
#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounterVector.h"
#include "../../../primitives/PerformanceCounter/Time/TimePerformanceCounter.h"

#include <vector>
#include <queue>
#include <assert.h>
#include <iostream>

#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>

/*
 *
 */
namespace pheet {

struct BasicMixedModeSchedulerPerformanceCounters {
	BasicMixedModeSchedulerPerformanceCounters(procs_t num_levels, procs_t num_threads)
	: num_tasks_at_level(num_levels),
	  num_steal_calls_per_thread(num_threads),
	  num_unsuccessful_steal_calls_per_thread(num_threads) {

	}

	BasicMixedModeSchedulerPerformanceCounters(BasicMixedModeSchedulerPerformanceCounters& other)
		: num_tasks_at_level(other.num_tasks_at_level),
		  num_steal_calls_per_thread(other.num_steal_calls_per_thread),
		  num_unsuccessful_steal_calls_per_thread(other.num_unsuccessful_steal_calls_per_thread),
		  num_spawns(other.num_spawns), num_spawns_to_call(other.num_spawns_to_call),
		  num_calls(other.num_calls), num_finishes(other.num_finishes),
		  num_steals(other.num_steals), num_steal_calls(other.num_steal_calls),
		  num_unsuccessful_steal_calls(other.num_unsuccessful_steal_calls),
		  num_stealing_deque_pop_cas(other.num_stealing_deque_pop_cas),
		  total_time(other.total_time), task_time(other.task_time),
		  sync_time(other.sync_time), idle_time(other.idle_time),
		  queue_processing_time(other.queue_processing_time),
		  visit_partners_time(other.visit_partners_time),
		  wait_for_finish_time(other.wait_for_finish_time),
		  wait_for_coordinator_time(other.wait_for_coordinator_time) {}

	BasicPerformanceCounterVector<scheduler_count_tasks_at_level> num_tasks_at_level;
	BasicPerformanceCounterVector<scheduler_count_steal_calls_per_thread> num_steal_calls_per_thread;
	BasicPerformanceCounterVector<scheduler_count_unsuccessful_steal_calls_per_thread> num_unsuccessful_steal_calls_per_thread;

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
	TimePerformanceCounter<scheduler_measure_sync_time> sync_time;
	TimePerformanceCounter<scheduler_measure_idle_time> idle_time;
	TimePerformanceCounter<scheduler_measure_queue_processing_time> queue_processing_time;
	TimePerformanceCounter<scheduler_measure_visit_partners_time> visit_partners_time;
	TimePerformanceCounter<scheduler_measure_wait_for_finish_time> wait_for_finish_time;
	TimePerformanceCounter<scheduler_measure_wait_for_coordinator_time> wait_for_coordinator_time;
};

union BasicMixedModeSchedulerTaskExecutionContextRegistration {
	uint64_t complete;
	struct {
	//	uint32_t c;	// counter
		uint32_t r;	// required threads
		uint32_t a;	// acquired threads
	} parts;
};

struct BasicMixedModeSchedulerTaskExecutionContextFinishStackElement {
	// Modified by local thread. Incremented when task is spawned, decremented when finished
	size_t num_spawned;

	// Only modified by other threads. Stolen tasks that were finished (including spawned tasks)
	size_t num_finished_remote;

	// Pointer to num_finished_remote of another thread (the one we stole tasks from)
	BasicMixedModeSchedulerTaskExecutionContextFinishStackElement* parent;

	BasicMixedModeSchedulerTaskExecutionContextFinishStackElement* prev_el;

	// Counter to update atomically when finalizing this element (ABA problem)
	size_t version;
};

/*
 * Information about the next task to execute
 */
//
template <class Task, class StackElement>
struct BasicMixedModeSchedulerTaskExecutionContextTeamTaskData {
	// Task to execute
	Task* task;

	// Pointer to the next task to execute. set by coordinator
	BasicMixedModeSchedulerTaskExecutionContextTeamTaskData<Task, StackElement>* next;

	// Parent finish_stack element needed for signaling finish
	StackElement* parent;

	// Threads count down as soon as they finish executing the task.
	// The last thread to count down is responsible for cleanup of the task (signal finish, delete task)
	procs_t executed_countdown;

	// Size of the team to execute the task
	procs_t team_size;

	// Level of the team to execute the task
	procs_t team_level;
};

template <class TaskExecutionContext>
struct BasicMixedModeSchedulerTaskExecutionContextTeamAnnouncement {
	typename TaskExecutionContext::TeamTaskData* first_task;
	typename TaskExecutionContext::TeamTaskData* last_task;

	TaskExecutionContext* coordinator;
	BasicMixedModeSchedulerTaskExecutionContextTeamAnnouncement<TaskExecutionContext>* next_team;

	typename TaskExecutionContext::Registration reg;
	procs_t level;
};


struct BasicMixedModeSchedulerTaskExecutionContextTeamInfo {
	procs_t team_size;
	procs_t local_id;
	procs_t coordinator_id;
	procs_t team_level;
};

template <class TaskExecutionContext>
struct BasicMixedModeSchedulerTaskExecutionContextLevelDescription {
	TaskExecutionContext** partners;
	procs_t num_partners;
	procs_t local_id;
	procs_t total_size;
	procs_t spawn_same_size_threshold;
	bool reverse_ids;
};

template <class TaskExecutionContext>
struct BasicMixedModeSchedulerTaskExecutionContextDequeItem {
	BasicMixedModeSchedulerTaskExecutionContextDequeItem();

	procs_t team_size;
	typename TaskExecutionContext::Task* task;
	typename TaskExecutionContext::FinishStackElement* finish_stack_element;

	bool operator==(BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const;
	bool operator!=(BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext> const& other) const;
};

template <class TaskExecutionContext>
BasicMixedModeSchedulerTaskExecutionContextDequeItem<TaskExecutionContext>::BasicMixedModeSchedulerTaskExecutionContextDequeItem()
: task(NULL), finish_stack_element(NULL) {

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
	typedef BasicMixedModeSchedulerTaskExecutionContextTeamTaskData<Task, FinishStackElement> TeamTaskData;
	typedef BasicMixedModeSchedulerTaskExecutionContextTeamInfo TeamInfo;
	typedef BasicMixedModeSchedulerTaskExecutionContextTeamAnnouncement<TaskExecutionContext> TeamAnnouncement;

	BasicMixedModeSchedulerTaskExecutionContext(std::vector<LevelDescription*> const* levels, std::vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state, BasicMixedModeSchedulerPerformanceCounters& perf_count);
	~BasicMixedModeSchedulerTaskExecutionContext();

	void join();

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void local_spawn(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void finish_nt(procs_t nt_size, TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call_nt(procs_t nt_size, TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn_nt(procs_t nt_size, TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void local_spawn_nt(procs_t nt_size, TaskParams&& ... params);

	// If team is out of sync
	void sync_team();
//	void team_barrier();

	bool is_coordinator();
	procs_t get_global_id();
	procs_t get_local_id();
	procs_t get_coordinator_id();
	procs_t get_team_size();
	procs_t get_max_team_size();

	boost::mt19937& get_rng();

private:
	void run();
	void init();

	void wait_for_shutdown();
	void wait_for_finish(FinishStackElement* parent);
	void wait_for_sync();
	void wait_for_coordinator_finish(TeamTaskData const* parent_task);
	void coordinate_team();
	bool coordinate_team_until_finished(FinishStackElement* parent);
	void coordinate_team_level();
	bool coordinate_team_level_until_finished(FinishStackElement* parent);
	void disband_team();
	bool execute_next_queue_task();
	bool execute_next_queue_task(procs_t min_level);
	void execute_queue_task(DequeItem const& di);
	void execute_solo_queue_task(DequeItem const& di);
	void execute_team_task(TeamTaskData* team_task);
	void create_team(procs_t team_size);
	TeamTaskData* create_team_task(DequeItem di);
	TeamTaskData* create_solo_team_task(DequeItem di);
	void announce_first_team_task(TeamTaskData* team_task);
	void announce_next_team_task(TeamTaskData* team_task);
	FinishStackElement* create_non_blocking_finish_region(FinishStackElement* parent);
	void empty_finish_stack();
	void signal_task_completion(FinishStackElement* finish_stack_element);
	void finalize_finish_stack_element(FinishStackElement* element, FinishStackElement* parent, size_t version);
	void visit_partners();
	void visit_partners_until_finished(FinishStackElement* parent);
	void visit_partners_until_synced(TeamAnnouncement* team);
	TeamAnnouncement* find_partner_team(TaskExecutionContext* partner, procs_t level);
	void join_team(TeamAnnouncement* team);
	bool tie_break_team(TeamAnnouncement* my_team, TeamAnnouncement* other_team);
	void follow_team();
	void prepare_team_info(TeamAnnouncement* team);
	void prepare_solo_team_info();
	procs_t get_level_for_num_threads(procs_t num_threads);
	bool has_local_work();
	bool has_local_work(procs_t min_level);
	DequeItem get_next_team_task();
	DequeItem get_next_queue_task();
	DequeItem get_next_queue_task(procs_t min_level);
	DequeItem steal_tasks_from_partner(TaskExecutionContext* partner, procs_t min_level);
	DequeItem steal_for_sync(TeamAnnouncement* my_team, TaskExecutionContext* partner, procs_t min_level);
	void store_item_in_deque(DequeItem di, procs_t level);

	void start_finish_region();
	void end_finish_region();

	void register_for_team(TeamAnnouncement* team);
	bool deregister_from_team(TeamAnnouncement* team);

	BasicMixedModeSchedulerPerformanceCounters performance_counters;

	// Stack is only used by the coordinator
	static size_t const finish_stack_size;
	FinishStackElement* finish_stack;
	size_t finish_stack_filled_left;
	size_t finish_stack_filled_right;
	size_t finish_stack_init_left;

	// machine hierarchy
	LevelDescription* levels;
	procs_t num_levels;
	procs_t* level_map;

	// Team task information
	TeamTaskData* current_team_task;
	TeamAnnouncement* current_team;
//	TeamAnnouncement** announced_teams;
//	procs_t team_announcement_index;

	// Information needed for task reclamation scheme
	// TODO: we might get rid of the queue alltogether and use a single pointer instead as the queue never gets more than one entry (i think)
	std::queue<TeamTaskData*> team_task_reclamation_queue;
	TeamTaskData** team_task_reuse;

	// Information needed for team reclamation scheme
	std::queue<TeamAnnouncement*> team_announcement_reclamation_queue;
	TeamAnnouncement** team_announcement_reuse;

	// for finishing
	TeamTaskData const* waiting_for_finish;

	// Information about the team (calculated by all threads)
	TeamInfo* team_info;
	TeamInfo* default_team_info;
	TeamInfo* solo_team_info;

	StealingDeque<DequeItem>** stealing_deques;
	StealingDeque<DequeItem>** lowest_level_deque;
	StealingDeque<DequeItem>** highest_level_deque;
//	StealingDeque<DequeItem>** current_deque;

	// Information relevant for execution
	CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque> > thread_executor;

	boost::mt19937 rng;

	typename Scheduler::State* scheduler_state;

	friend class CPUThreadExecutor<typename CPUHierarchy::CPUDescriptor, BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>>;
	friend class Scheduler::Finish;
};

template <class Scheduler, template <typename T> class StealingDeque>
size_t const BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish_stack_size = 2048;


template <class Scheduler, template <typename T> class StealingDeque>
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::BasicMixedModeSchedulerTaskExecutionContext(std::vector<LevelDescription*> const* levels, std::vector<typename CPUHierarchy::CPUDescriptor*> const* cpus, typename Scheduler::State* scheduler_state, BasicMixedModeSchedulerPerformanceCounters& perf_count)
: performance_counters(perf_count),
  finish_stack_filled_left(0), finish_stack_filled_right(finish_stack_size), finish_stack_init_left(0), num_levels(levels->size()), current_team_task(NULL), current_team(NULL),
  /*team_announcement_index(0),*/ waiting_for_finish(NULL), team_info(NULL), lowest_level_deque(NULL),
  highest_level_deque(NULL)/*, current_deque(NULL)*/, thread_executor(cpus, this), scheduler_state(scheduler_state)
  {
	performance_counters.total_time.start_timer();

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

	// May be moved to the parallel part if we add a full barrier after initialization. Not sure if it is worth it though
	stealing_deques = new StealingDeque<DequeItem>*[num_levels];
	for(procs_t i = 0; i < num_levels; ++i) {
		procs_t size = find_last_bit_set(this->levels[num_levels - i - 1].total_size) << 4;
		stealing_deques[i] = new StealingDeque<DequeItem>(size, performance_counters.num_steals, performance_counters.num_stealing_deque_pop_cas);
		this->levels[i].spawn_same_size_threshold = size;
	}

	// All other initializations can be done in parallel

	// Run thread
	thread_executor.run();
}

template <class Scheduler, template <typename T> class StealingDeque>
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::~BasicMixedModeSchedulerTaskExecutionContext() {
	delete[] finish_stack;
	delete[] levels;
	delete[] level_map;
//	delete[] announced_teams;

	delete default_team_info;
	delete solo_team_info;

	while(!team_task_reclamation_queue.empty()) {
		delete team_task_reclamation_queue.front();
		team_task_reclamation_queue.pop();
	}

	while(!team_announcement_reclamation_queue.empty()) {
		delete team_announcement_reclamation_queue.front();
		team_announcement_reclamation_queue.pop();
	}

	for(procs_t i = 0; i < num_levels; ++i) {
		delete team_task_reuse[i];
		delete team_announcement_reuse[i];
		delete stealing_deques[i];
	}
	delete[] team_task_reuse;
	delete[] team_announcement_reuse;
	delete[] stealing_deques;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::join() {
	thread_executor.join();
}

/*
 * Initialization of stuff that can be done in parallel
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::init() {
	finish_stack = new FinishStackElement[finish_stack_size];

	// Create map for simple lookup of levels in the hierarchy (if we have a number of threads)
	procs_t max_levels = find_last_bit_set(this->levels[0].total_size - 1) + 1;
	level_map = new procs_t[max_levels];
	procs_t lvl = num_levels - 1;
	level_map[0] = num_levels - 1;
	for(procs_t i = 1; i < max_levels - 1; i++) {
		procs_t min_size = (1 << (i - 1)) + 1;
		procs_t lvl_size = this->levels[lvl].total_size;
		while(min_size > lvl_size) {
			--lvl;
			lvl_size = this->levels[lvl].total_size;
		}
		level_map[i] = lvl;
	}
	level_map[max_levels - 1] = 0;

	// Initialize stuff for memory reuse
//	announced_teams = new TeamAnnouncement*[num_levels];
	team_task_reuse = new TeamTaskData*[num_levels];
	team_announcement_reuse = new TeamAnnouncement*[num_levels];
	for(procs_t i = 0; i < num_levels; ++i) {
//		announced_teams[i] = NULL;

		team_task_reuse[i] = new TeamTaskData();
		team_announcement_reuse[i] = new TeamAnnouncement();
	}
	team_task_reuse[num_levels - 1]->team_level = num_levels - 1;
	team_task_reuse[num_levels - 1]->team_size = 1;
	team_task_reuse[num_levels - 1]->next = NULL;
	team_task_reuse[num_levels - 1]->executed_countdown = 0;
	team_announcement_reuse[num_levels - 1]->level = num_levels - 1;
	team_announcement_reuse[num_levels - 1]->coordinator = this;

	// Initialize team_info structure
	default_team_info = new TeamInfo();

	solo_team_info = new TeamInfo();
	solo_team_info->coordinator_id = 0;
	solo_team_info->local_id = 0;
	solo_team_info->team_level = num_levels - 1;
	solo_team_info->team_size = 1;

	team_info = solo_team_info;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::run() {
	init();

	// Wait until all threads are initialized
	scheduler_state->state_barrier.wait(0, 1);

	// Start execution
	Task* startup_task = scheduler_state->startup_task;
	if(startup_task != NULL && PTR_CAS(&(scheduler_state->startup_task), startup_task, NULL)) {
		performance_counters.queue_processing_time.start_timer();
		start_finish_region();

		++finish_stack[finish_stack_filled_right].num_spawned;
		DequeItem di;
		di.finish_stack_element = &(finish_stack[finish_stack_filled_right]);
		di.task = startup_task;
		di.team_size = scheduler_state->team_size;

		create_team(di.team_size);

		performance_counters.queue_processing_time.stop_timer();
		if(di.team_size == 1) {
			execute_solo_queue_task(di);
		}
		else {
			execute_queue_task(di);
		}
		performance_counters.queue_processing_time.start_timer();

		if(di.finish_stack_element->num_spawned > 0) {
			// There exist some non-executed or stolen tasks

			// Execute the rest of the tasks in the team
			coordinate_team();

			performance_counters.queue_processing_time.stop_timer();

			// Process other tasks until this task has been finished
			wait_for_finish(di.finish_stack_element);
		}
		else {
			performance_counters.queue_processing_time.stop_timer();
		}
		if(current_team != NULL && current_team->level != num_levels - 1) {
			disband_team();
		}

		scheduler_state->current_state = 2; // finished
	}
	else {
		wait_for_shutdown();
	}

	// We need to stop here, as it isn't safe after the barrier
	performance_counters.total_time.stop_timer();

	scheduler_state->state_barrier.barrier(1, levels[0].total_size);

	// Now we can safely finish execution
}

/*
 * Do work until the scheduler shuts down
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::wait_for_shutdown() {
	// pre-condition: queue must be empty
	assert(!has_local_work());

	while(scheduler_state->current_state != 2) {
		// Try to join teams or steal tasks
		visit_partners();

		performance_counters.queue_processing_time.start_timer();
		// Coordinate the current team if existing until it's empty
		coordinate_team();

		while(has_local_work()) {
			// Execute a single task. This will create a team if there was a task
			if(execute_next_queue_task()) {
				// Coordinate the current team if existing until it's empty
				coordinate_team();
			}
		}
		performance_counters.queue_processing_time.stop_timer();
	}
}

/*
 * Do work until the task has been finished
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::wait_for_finish(FinishStackElement* parent) {
	while(parent->num_finished_remote != parent->num_spawned) {
		// TODO: try a policy where we do not need to empty our queues before we notice the finish
		// (currently not implemented for simplicity)

		performance_counters.queue_processing_time.start_timer();
		while(has_local_work()) {
			// Execute a single task. This will create a team if there was a task
			if(execute_next_queue_task()) {
				// Coordinate the current team if existing until it's empty
				if(coordinate_team_until_finished(parent)) {
					performance_counters.queue_processing_time.stop_timer();
					return;
				}
			}
		}
		performance_counters.queue_processing_time.stop_timer();

		// Try to join teams or steal tasks
		visit_partners_until_finished(parent);

		performance_counters.queue_processing_time.start_timer();
		// Coordinate the current team if existing until it's empty
		if(coordinate_team_until_finished(parent)) {
			performance_counters.queue_processing_time.stop_timer();
			return;
		}
		performance_counters.queue_processing_time.stop_timer();
	}
}

/*
 * Do work until the current team is synchronized
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::wait_for_sync() {
	// TODO: make this work if a finish is invoked inside sync with thread requirement >= this

	// TODO: Let threads find the big team even though it is shadowed by a temporary team - (presumably higher performance)

	procs_t min_level = current_team->level + 1;
	TeamAnnouncement* my_team = current_team;
	TeamTaskData* my_team_task = current_team_task;
	TeamInfo* my_team_info = default_team_info;
	TeamInfo sub_team_info;
	default_team_info = &sub_team_info;
	do {
		if(has_local_work(min_level)) {
			if(!deregister_from_team(my_team)) {
				break;
			}

			performance_counters.queue_processing_time.start_timer();
			do {
				performance_counters.sync_time.stop_timer();
				// Execute a single task. This will create a team if there was a task
				if(execute_next_queue_task(min_level)) {
					// Coordinate the current team if existing until it's empty
					coordinate_team();
				}
				else {
					performance_counters.queue_processing_time.start_timer();
				}
				performance_counters.sync_time.start_timer();
			} while(has_local_work(min_level));
			performance_counters.queue_processing_time.stop_timer();
			// If we executed something, make sure we drop the team

			if(current_team != my_team && current_team != NULL && current_team->level != num_levels - 1) {
				disband_team();
			}

			register_for_team(my_team);
		//	--team_announcement_index;

			if(my_team->reg.parts.a == my_team->reg.parts.r) {
				break;
			}
		}

		assert(!has_local_work(min_level));

		// steal tasks from partners or join partner teams
		visit_partners_until_synced(my_team);

		if(current_team != my_team) {
			performance_counters.queue_processing_time.start_timer();
			// Coordinate the current team if existing until it's empty
			coordinate_team();
			if(current_team != NULL && current_team->level != num_levels - 1) {
				disband_team();
			}
			performance_counters.queue_processing_time.stop_timer();

			register_for_team(my_team);
		}
	}while(my_team->reg.parts.a != my_team->reg.parts.r);
	default_team_info = my_team_info;
	team_info = my_team_info;
//	current_team = my_team;
	current_team_task = my_team_task;
}

/*
 * Do work until the task has been finished
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::wait_for_coordinator_finish(TeamTaskData const* parent_task) {
	TeamTaskData const* prev_waiting_for_finish = waiting_for_finish;
	waiting_for_finish = parent_task;

	follow_team();

	waiting_for_finish = prev_waiting_for_finish;
}

/*
 * Coordinates a team until we run out of work for the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::coordinate_team() {
	if(current_team != NULL) {
		procs_t level = current_team->level;
		coordinate_team_level();

		// TODO: get smaller tasks and just resize the team instead of disbanding it

		if(level < num_levels - 1) {
			disband_team();
		}
		else {
			current_team = NULL;
		}
	}
}

/*
 * Coordinates a team until we run out of work for the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::coordinate_team_until_finished(FinishStackElement* parent) {
	if(current_team != NULL) {
		procs_t level = current_team->level;
		if(coordinate_team_level_until_finished(parent)) {
			return true;
		}

		// TODO: get smaller tasks and just resize the team instead of disbanding it

		if(level < num_levels - 1) {
			disband_team();
		}
		else {
			current_team = NULL;
		}
	}
	return false;
}

/*
 * Coordinates a team until we run out of work for the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::coordinate_team_level() {
	if(current_team->level == num_levels - 1) {
		// Solo team
		DequeItem di = get_next_team_task();
		while(di.task != NULL) {
			performance_counters.queue_processing_time.stop_timer();
			// Execute
			execute_solo_queue_task(di);
			performance_counters.queue_processing_time.start_timer();

			// Try to get a same-size task
			di = get_next_team_task();
		}
		current_team = NULL;
	}
	else {
		DequeItem di = get_next_team_task();
		while(di.task != NULL) {
			// This method is responsible for creating the team task data and finish stack elements, etc...
			TeamTaskData* tt = create_team_task(di);

			// Show it to the rest of the team
			announce_next_team_task(tt);

			performance_counters.queue_processing_time.stop_timer();
			// Execute (same for coor and client!)
			execute_team_task(tt);
			performance_counters.queue_processing_time.start_timer();

			// Send task to memory reclamation
			team_task_reclamation_queue.push(tt);

			// Try to get a same-size task
			di = get_next_team_task();
		}
	}
}

/*
 * Coordinates a team until we run out of work for the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::coordinate_team_level_until_finished(FinishStackElement* parent) {
	if(current_team->level == num_levels - 1) {
		// Solo team
		DequeItem di = get_next_team_task();
		while(di.task != NULL) {
			performance_counters.queue_processing_time.stop_timer();
			// Execute
			execute_solo_queue_task(di);
			performance_counters.queue_processing_time.start_timer();

			if(parent->num_spawned == parent->num_finished_remote) {
				return true;
			}

			// Try to get a same-size task
			di = get_next_team_task();
		}
		current_team = NULL;
	}
	else {
		DequeItem di = get_next_team_task();
		while(di.task != NULL) {
			// This method is responsible for creating the team task data and finish stack elements, etc...
			TeamTaskData* tt = create_team_task(di);

			// Show it to the rest of the team
			announce_next_team_task(tt);

			performance_counters.queue_processing_time.stop_timer();
			// Execute (same for coor and client!)
			execute_team_task(tt);
			performance_counters.queue_processing_time.start_timer();

			// Send task to memory reclamation
			team_task_reclamation_queue.push(tt);

			if(parent->num_spawned == parent->num_finished_remote) {
				return true;
			}

			// Try to get a same-size task
			di = get_next_team_task();
		}
	}
	return false;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::disband_team() {
	assert(current_team != NULL);
	assert(current_team->level != num_levels - 1);
	assert(current_team->coordinator == this);
	assert(current_team->reg.parts.r == current_team->reg.parts.a);

	// Put the old team into memory reclamation (as soon as it will be retrieved from reclamation,
	// it is guaranteed that no other (relevant) threads still have a reference to this
	team_announcement_reclamation_queue.push(current_team);

	// Create a dummy team with a single thread - Other threads will see this team and exit
	create_team(1);

	// Finally, drop out of this team
	current_team = NULL;
}

/*
 * Finds a single task, creates a team for it and executes the task
 */
template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_next_queue_task() {
	DequeItem di = get_next_queue_task();

	if(di.task != NULL) {
		performance_counters.queue_processing_time.stop_timer();
		create_team(di.team_size);

		if(di.team_size == 1) {
			execute_solo_queue_task(di);
		}
		else {
			execute_queue_task(di);
		}
		performance_counters.queue_processing_time.start_timer();
		return true;
	}
	return false;
}

/*
 * Finds a single task, creates a team for it and executes the task
 */
template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_next_queue_task(procs_t min_level) {
	DequeItem di = get_next_queue_task(min_level);

	if(di.task != NULL) {
		performance_counters.queue_processing_time.stop_timer();
		create_team(di.team_size);

		if(di.team_size == 1) {
			execute_solo_queue_task(di);
		}
		else {
			execute_queue_task(di);
		}
		performance_counters.queue_processing_time.start_timer();
		return true;
	}
	return false;
}

/*
 * executes the given task (if it is a team task, it is announced for the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_queue_task(DequeItem const& di) {
/*	if(di.team_size == 1) {
		execute_solo_task(di);
	}
	else {*/
	assert(di.team_size > 1);

	// This method is responsible for creating the team task data and finish stack elements, etc...
	TeamTaskData* tt = create_team_task(di);

	// Show it to the rest of the team
	announce_first_team_task(tt);

	// Execute (same for coor and client!)
	execute_team_task(current_team_task);

	// Send task to memory reclamation
	team_task_reclamation_queue.push(tt);
//	}
}

/*
 * executes the given task (if it is a team task, it is announced for the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_solo_queue_task(DequeItem const& di) {
	assert(di.team_size == 1);
	performance_counters.num_tasks_at_level.incr(num_levels - 1);

	current_team_task = create_solo_team_task(di);

	performance_counters.task_time.start_timer();
	// Execute task
	(*di.task)(*this);
	performance_counters.task_time.stop_timer();

	// signal that we finished execution of the task
	signal_task_completion(current_team_task->parent);

	delete di.task;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::execute_team_task(TeamTaskData* team_task) {
	performance_counters.task_time.start_timer();
	// Execute task
	(*(team_task->task))(*this);

	// Signal that we finished executing this task
	if(PROCST_FETCH_AND_SUB(&(team_task->executed_countdown), 1) == 1) {
		// Last task to finish - do some cleanup

		// signal that we finished execution of the task
		signal_task_completion(team_task->parent);

		delete team_task->task;
	}
	performance_counters.task_time.stop_timer();
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::create_team(procs_t team_size) {
	procs_t level = get_level_for_num_threads(team_size);
	if(level == num_levels - 1) {
		TeamAnnouncement* team = team_announcement_reuse[num_levels - 1];
		if(current_team != NULL) {
			// First announce the next team so other threads in the current team know that this team is finished (which means they will call sync_team)
			current_team->last_task = current_team_task;
			current_team->next_team = team;

			// We have to make sure the team is synced first before changing the current_team pointer
			// New team smaller: make sure all threads joined the team (last chance for getting those threads in)
			sync_team();
//			announced_teams[team_announcement_index] = NULL;
		}
		current_team = team;
	//	current_deque = stealing_deques + num_levels - 1;
		prepare_solo_team_info();
	}
	else {
		TeamAnnouncement* team;

		if(!team_announcement_reclamation_queue.empty()) {
			assert(team_announcement_reclamation_queue.front()->reg.parts.r == team_announcement_reclamation_queue.front()->reg.parts.a);
			assert(team_announcement_reclamation_queue.front()->level < num_levels - 1);

			TeamAnnouncement* tmp = team_announcement_reclamation_queue.front();
			team_announcement_reclamation_queue.pop();
			team = team_announcement_reuse[tmp->level];
			team_announcement_reuse[tmp->level] = tmp;
		}
		else {
			team = new TeamAnnouncement();
		}

		team->coordinator = this;
		team->first_task = NULL;
		team->last_task = NULL;
		team->level = level;
		team->next_team = NULL;
		team->reg.parts.r = levels[level].total_size;
		if(current_team == NULL) {
			team->reg.parts.a = 1;
		}
		else {


			if(current_team->level > team->level) {
				team->reg.parts.a = current_team->reg.parts.r;
			}
			else {
				// Synchronized from the start - perfect
				team->reg.parts.a = levels[level].total_size;
			}
		}

		// TODO: If we have to sync the team, we can drop the fence
		MEMORY_FENCE();

		if(current_team != NULL) {
		/*	if(current_team->level > team->level) {
				// We now need more threads so we need to reannounce.
				announced_teams[team_announcement_index] = team;
			}
			else {
				assert(current_team->level != team->level);
				// This team is smaller so no need for reannouncement
				announced_teams[team_announcement_index] = NULL;
			}*/
			// First announce the next team so other threads in the current team know that this team is finished (which means they will call sync_team)
			current_team->last_task = current_team_task;
			current_team->next_team = team;

			// We have to make sure the team is synced first before changing the current_team pointer
			// New team smaller: make sure all threads joined the team (last chance for getting those threads in)
			// New team larger: Make sure all threads saw the announcement
			sync_team();
		}
	/*	else {
			announced_teams[team_announcement_index] = team;
		}*/
		current_team = team;
	//	current_deque = stealing_deques + level;

		prepare_team_info(team);
	}
}

/*
 * Creates a new team task with the given parameters.
 *
 * Do not create single-threaded tasks with this
 */
template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::TeamTaskData*
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::create_team_task(DequeItem di) {
	TeamTaskData* team_task;

	assert(di.team_size != 1);

	if(!team_task_reclamation_queue.empty() && team_task_reclamation_queue.front()->executed_countdown == 0) {
		TeamTaskData* tmp = team_task_reclamation_queue.front();
		team_task_reclamation_queue.pop();
		team_task = team_task_reuse[tmp->team_level];
		team_task_reuse[tmp->team_level] = tmp;
	}
	else {
		team_task = new TeamTaskData();
	}

	FinishStackElement* parent = di.finish_stack_element;
	if(parent < finish_stack || (parent >= (finish_stack + finish_stack_size))) {
		// to prevent thrashing on the parent finish block, we create a new finish block local to the coordinator
		// TODO: evaluate whether we can let the coordinator update the finish block and just do perform some checks
		// when it should be completed

		// Perform cleanup on finish stack
		empty_finish_stack();

		// Create new stack element for finish
		parent = create_non_blocking_finish_region(parent);
	}

	team_task->team_level = current_team->level;
	team_task->team_size = levels[current_team->level].total_size; //nt_size;
	team_task->task = di.task;
	team_task->parent = parent;
	team_task->next = NULL;
	team_task->executed_countdown = levels[current_team->level].total_size;

	// Make sure all changes are written before it will be published
	MEMORY_FENCE();

	return team_task;
}

/*
 * Creates a new team task with the given parameters. for solo execution
 *
 * Do not create multi-threaded tasks with this
 */
template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::TeamTaskData*
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::create_solo_team_task(DequeItem di) {
	assert(di.team_size == 1);

	TeamTaskData* team_task = team_task_reuse[num_levels - 1];

	FinishStackElement* parent = di.finish_stack_element;
	if(parent < finish_stack || (parent >= (finish_stack + finish_stack_size))) {
		// to prevent thrashing on the parent finish block, we create a new finish block local to the coordinator
		// TODO: evaluate whether we can let the coordinator update the finish block and just do perform some checks
		// when it should be completed

		// Perform cleanup on finish stack
		empty_finish_stack();

		// Create new stack element for finish
		parent = create_non_blocking_finish_region(parent);
	}

	team_task->task = di.task;
	team_task->parent = parent;

	return team_task;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::announce_first_team_task(TeamTaskData* team_task) {
	performance_counters.num_tasks_at_level.incr(team_task->team_level);

	current_team->first_task = team_task;
	current_team_task = team_task;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::announce_next_team_task(TeamTaskData* team_task) {
	performance_counters.num_tasks_at_level.incr(team_task->team_level);

	current_team_task->next = team_task;
	current_team_task = team_task;
}

template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::FinishStackElement*
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::create_non_blocking_finish_region(FinishStackElement* parent) {
	assert(finish_stack_filled_left < finish_stack_size);

	finish_stack[finish_stack_filled_left].num_finished_remote = 0;
	// As we create it out of a parent region that is waiting for completion of a single task, we can already add this one task here
	finish_stack[finish_stack_filled_left].num_spawned = 1;
	finish_stack[finish_stack_filled_left].parent = parent;

	if(finish_stack_filled_left >= finish_stack_init_left/* && stack_filled_left < stack_init_right*/) {
		finish_stack[finish_stack_filled_left].version = 0;
		++finish_stack_init_left;
	}

	++finish_stack_filled_left;

	return &(finish_stack[finish_stack_filled_left - 1]);
}

/*
 * empty finish_stack but not below limit
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::empty_finish_stack() {
	while(finish_stack_filled_left > 0) {
		size_t se = finish_stack_filled_left - 1;
		if(finish_stack[se].num_spawned == finish_stack[se].num_finished_remote &&
				finish_stack[finish_stack_filled_left].parent == NULL) {
		//	finalize_finish_stack_element(&(finish_stack[se]), finish_stack[se].parent);

			finish_stack_filled_left = se;

			// When parent is set to NULL, some thread is finalizing/has finalized this stack element (otherwise we would have an error)
			// Actually we have to check before whether parent has already been set to NULL, or we might have a race
		//	assert(finish_stack[finish_stack_filled_left].parent == NULL);

		}
		else {
			break;
		}
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::signal_task_completion(FinishStackElement* finish_stack_element) {
	FinishStackElement* parent = finish_stack_element->parent;
	size_t version = finish_stack_element->version;

	if(finish_stack_element >= finish_stack && (finish_stack_element < (finish_stack + finish_stack_size))) {
		assert(finish_stack_element->num_spawned > 0);
		--(finish_stack_element->num_spawned);

		// Memory fence is unfortunately required to guarantee that some thread finalizes the finish_stack_element
		// TODO: prove that the fence is enough
		MEMORY_FENCE();
	}
	else {
		// Increment num_finished_remote of parent
		SIZET_ATOMIC_ADD(&(finish_stack_element->num_finished_remote), 1);
	}
	if(finish_stack_element->num_spawned == finish_stack_element->num_finished_remote) {
		finalize_finish_stack_element(finish_stack_element, parent, version);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finalize_finish_stack_element(FinishStackElement* element, FinishStackElement* parent, size_t version) {
	if(parent != NULL) {
		if(element->num_spawned == 0) {
			// No tasks processed remotely - no need for atomic ops
		//	element->parent = NULL;
			++(element->version);
			signal_task_completion(parent);
		}
		else {
			if(PTR_CAS(&(element->version), version, version + 1)) {
				signal_task_completion(parent);
			}
		}
	}
}

/*
 * Stealing routine for idle threads
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::visit_partners() {
	// If we still have local work, this might never terminate
	assert(!has_local_work());

	performance_counters.idle_time.start_timer();
	performance_counters.visit_partners_time.start_timer();

	Backoff bo;
	DequeItem di;
	while(true) {
		// We do not steal from the last level as there are no partners
		procs_t level = num_levels - 1;
		while(level > 0) {
			--level;
			// For all except the last level we assume num_partners > 0
			assert(levels[level].num_partners > 0);
			boost::uniform_int<procs_t> n_r_gen(0, levels[level].num_partners - 1);
			procs_t next_rand = n_r_gen(rng);
			assert(next_rand < levels[level].num_partners);
			TaskExecutionContext* partner = levels[level].partners[next_rand];
			assert(partner != this);

			TeamAnnouncement* team = find_partner_team(partner, level);
			if(team != NULL) {
				// Joins the team and executes all tasks. Only returns after the team has been disbanded
				performance_counters.visit_partners_time.stop_timer();
				performance_counters.idle_time.stop_timer();

				join_team(team);
				assert(current_team == NULL);
				return;
			}

			di = steal_tasks_from_partner(partner, level + 1);
		//	di = levels[level].partners[next_rand % levels[level].num_partners]->stealing_deque.steal();

			if(di.task != NULL) {
				performance_counters.visit_partners_time.stop_timer();
				performance_counters.idle_time.stop_timer();

				create_team(di.team_size);

				if(di.team_size == 1) {
					execute_solo_queue_task(di);
				}
				else {
					execute_queue_task(di);
				}
				return;
			}
		}
		if(scheduler_state->current_state >= 2) {
			performance_counters.visit_partners_time.stop_timer();
			performance_counters.idle_time.stop_timer();
			return;
		}
		// We may perform the cleanup now, as we have to wait anyway (and this also makes debugging easier)
		empty_finish_stack();
		bo.backoff();
	}
}

/*
 * Stealing routine for (coordinating) threads waiting for a finish
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::visit_partners_until_finished(FinishStackElement* parent) {
	// If we still have local work, this might never terminate
	assert(!has_local_work());

	performance_counters.idle_time.start_timer();
	performance_counters.wait_for_finish_time.start_timer();

	Backoff bo;
	DequeItem di;
	while(true) {
		// We do not steal from the last level as there are no partners
		procs_t level = num_levels - 1;
		while(level > 0) {
			--level;
			// For all except the last level we assume num_partners > 0
			assert(levels[level].num_partners > 0);
			boost::uniform_int<procs_t> n_r_gen(0, levels[level].num_partners - 1);
			procs_t next_rand = n_r_gen(rng);
			assert(next_rand < levels[level].num_partners);
			TaskExecutionContext* partner = levels[level].partners[next_rand];
			assert(partner != this);

			TeamAnnouncement* team = find_partner_team(partner, level);
			if(team != NULL) {
				performance_counters.idle_time.stop_timer();
				performance_counters.wait_for_finish_time.stop_timer();

				// Joins the team and executes all tasks. Only returns after the team has been disbanded
				join_team(team);
				assert(current_team == NULL);
				return;
			}

			di = steal_tasks_from_partner(partner, level + 1);
		//	di = levels[level].partners[next_rand % levels[level].num_partners]->stealing_deque.steal();

			if(di.task != NULL) {
				performance_counters.idle_time.stop_timer();
				performance_counters.wait_for_finish_time.stop_timer();

				create_team(di.team_size);

				if(di.team_size == 1) {
					execute_solo_queue_task(di);
				}
				else {
					execute_queue_task(di);
				}
				return;
			}
		}
		if(parent->num_finished_remote == parent->num_spawned) {
			performance_counters.idle_time.stop_timer();
			performance_counters.wait_for_finish_time.stop_timer();

			return;
		}
		// We may perform the cleanup now, as we have to wait anyway (and this also makes debugging easier)
		empty_finish_stack();
		bo.backoff();
	}
}

/*
 * Stealing routine for (coordinating) threads waiting for all threads to join a team
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::visit_partners_until_synced(TeamAnnouncement* my_team_announcement) {
	procs_t min_level = my_team_announcement->level + 1;
	// If we still have local work, this might never terminate
	assert(!has_local_work(min_level));

	performance_counters.idle_time.start_timer();

	Backoff bo;
	DequeItem di;
	while(true) {
		// We do not steal from the last level as there are no partners
		procs_t level = num_levels - 1;
		while(level >= min_level) {
			--level;
			// For all except the last level we assume num_partners > 0
			assert(levels[level].num_partners > 0);
			boost::uniform_int<procs_t> n_r_gen(0, levels[level].num_partners - 1);
			procs_t next_rand = n_r_gen(rng);
			assert(next_rand < levels[level].num_partners);
			TaskExecutionContext* partner = levels[level].partners[next_rand];
			assert(partner != this);

			TeamAnnouncement* team = find_partner_team(partner, level);
			if(team != NULL) {
				// Joins the other team if it wins the tie-breaking scheme and executes all tasks. Only returns after the team has been disbanded
				performance_counters.idle_time.stop_timer();
				if(tie_break_team(my_team_announcement, team)) {

					// True means the other team won (false means we are still stuck with our team)
					return;
				}
				performance_counters.idle_time.start_timer();
			}

			// includes deregistration mechanism and stealing can only work if deregistration was successful
			// and we stay registered if nothing was stolen
			di = steal_for_sync(my_team_announcement, partner, level + 1);

			if(di.task != NULL) {
				performance_counters.sync_time.stop_timer();
				performance_counters.idle_time.stop_timer();

				create_team(di.team_size);

				if(di.team_size == 1) {
					execute_solo_queue_task(di);
				}
				else {
					execute_queue_task(di);
				}
				performance_counters.sync_time.start_timer();
				return;
			}
		}
		if(my_team_announcement->reg.parts.a == my_team_announcement->reg.parts.r) {
			performance_counters.idle_time.stop_timer();

			return;
		}
		bo.backoff();
	}
}

/*
 * Checks if the partner has a relevant team for this thread
 * This method assumes we are not bound to a team, another more complex method (with tie-breaking, etc.) is used during team sync
 */
template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::TeamAnnouncement*
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::find_partner_team(TaskExecutionContext* partner, procs_t level) {
	TeamAnnouncement* team = partner->current_team;
	if(team != NULL && team->level <= level) {
		// Make sure we haven't found an old announcement (in that case the team must already be fully synced)
		if(team->reg.parts.a == team->reg.parts.r) {
			// Old announcement - leave it
			return NULL;
		}
		return team;
	}
	return NULL;
}

/*
 * Joins the team and executes all tasks. Only returns after the team has been disbanded
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::join_team(TeamAnnouncement* team) {
	// Announce the team so it is visible to others
//	announced_teams[team_announcement_index] = team;
	current_team = team;

	register_for_team(team);
	assert(team->level == 0 || team->coordinator->levels[team->level - 1].partners == levels[team->level - 1].partners);

	// Calculate the team_level at which we have to drop out
/*	{
		assert(levels[team->level].local_id != team->coordinator->levels[team->level].local_id);

		TaskExecutionContext* smaller;
		TaskExecutionContext* larger;
		if(levels[team->level].local_id < team->coordinator->levels[team->level].local_id) {
			smaller = this;
			larger = team->coordinator;
		}
		else {
			smaller = team->coordinator;
			larger = this;
		}
		assert(smaller != larger);
		procs_t diff = larger->levels[team->level].local_id - smaller->levels[team->level].local_id;
		assert(diff > 0);
		procs_t lvl = team->level + 1;
		while(smaller->levels[lvl].local_id + diff == larger->levels[lvl].local_id) {
			++lvl;
			assert(lvl < smaller->num_levels && lvl < larger->num_levels);
		}
		max_team_level = lvl - 1;
	}*/

//	assert(current_team->level <= max_team_level);

	prepare_team_info(team);

	follow_team();

	assert(current_team == NULL);
}

/*
 * Joins the team and executes all tasks. Only returns after the team has been disbanded
 */
template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::tie_break_team(TeamAnnouncement* my_team, TeamAnnouncement* other_team) {
	if(my_team->level == other_team->level && my_team <= other_team) {
		// If both teams are the same or if they are at same level and this pointer is smaller - ignore the other
		return false;
	}
	if(my_team->level > other_team->level) {
		// Teams at higher levels are executed first
		return false;
	}
	// Tie breaking complete now switch to the new team
#ifndef NDEBUG
	bool dereg =
#endif
	deregister_from_team(my_team);
#ifndef NDEBUG
	// deregistration should never fail in this case, as my_team can't be completed if other_team wins (at least 1 thread would never join my_team)
	// TODO: This might change if we allow tasks with higher thread requirements to win over smaller ones in certain cases. Recheck it then
	assert(dereg);
#endif

	performance_counters.sync_time.stop_timer();
	join_team(other_team);
	performance_counters.sync_time.start_timer();
	assert(current_team == NULL);

	register_for_team(my_team);
	return true;
}

/*
 * Joins the team and executes all tasks. Only returns after the team has been disbanded or the task to finish has come up
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::follow_team() {
	performance_counters.queue_processing_time.start_timer();

	while(true) {
		{
			performance_counters.wait_for_coordinator_time.start_timer();
			Backoff bo;
			while(current_team->first_task == NULL) {
				bo.backoff();
			}
			performance_counters.wait_for_coordinator_time.stop_timer();
			current_team_task = current_team->first_task;
			if(current_team_task == waiting_for_finish) {
				performance_counters.queue_processing_time.stop_timer();
				return;
			}

			performance_counters.queue_processing_time.stop_timer();
			execute_team_task(current_team_task);
			performance_counters.queue_processing_time.start_timer();
		}

		while(current_team->last_task != current_team_task) {
			performance_counters.wait_for_coordinator_time.start_timer();
			Backoff bo;
			while(current_team_task->next == NULL && current_team->last_task != current_team_task) {
				bo.backoff();
			}
			performance_counters.wait_for_coordinator_time.stop_timer();
			if(current_team_task->next == NULL) {
				break;
			}
			current_team_task = current_team_task->next;
			if(current_team_task == waiting_for_finish) {
				performance_counters.queue_processing_time.stop_timer();
				return;
			}

			performance_counters.queue_processing_time.stop_timer();
			execute_team_task(current_team_task);
			performance_counters.queue_processing_time.start_timer();
		}

		{
			// We have to make sure the team is synced first
			// New team smaller: Make sure all threads joined the previous team (last chance for getting those threads in)
			// New team larger: Make sure all threads saw the previous announcement
			sync_team();

			// Wait for next team to appear
			Backoff bo;
			while(current_team->next_team == NULL) {
				bo.backoff();
			}
			// Announce new team if necessary
		/*	if(current_team->level > current_team->next_team->level) {
				announced_teams[team_announcement_index] = current_team->next_team;
			}
			else {
				announced_teams[team_announcement_index] = NULL;
			}*/

			// Update team information
			current_team = current_team->next_team;
			if(current_team->level >= num_levels ||
					(current_team->coordinator->levels[0].local_id - current_team->coordinator->levels[current_team->level].local_id)
					!= (levels[0].local_id - levels[current_team->level].local_id)) {
				current_team = NULL;
				performance_counters.queue_processing_time.stop_timer();
				break;
			}

			prepare_team_info(current_team);
		}
	}
}

/*
 * Calculates all information needed for the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::prepare_team_info(TeamAnnouncement* team) {
	assert(team != NULL);
	assert(team->level != num_levels - 1);
	team_info = default_team_info;
	team_info->team_level = team->level;
	team_info->team_size = levels[team->level].total_size;
	if(team->coordinator->levels[team->level].reverse_ids) {
		procs_t offset = team->coordinator->levels[team->level].total_size - 1;
		team_info->coordinator_id = offset - team->coordinator->levels[team->level].local_id;
		team_info->local_id = offset - levels[team->level].local_id;
	}
	else {
		team_info->coordinator_id = team->coordinator->levels[team->level].local_id;
		team_info->local_id = levels[team->level].local_id;
	}
}

/*
 * Calculates all information needed for the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::prepare_solo_team_info() {
	team_info = solo_team_info;
}

/*
 * Performs a synchronization of the team
 * After sync_team it is ensured, that all threads necessary for the team are working in the team
 */
template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::sync_team() {
	if(current_team->reg.parts.a != current_team->reg.parts.r) {
		performance_counters.task_time.stop_timer();
		performance_counters.sync_time.start_timer();
		wait_for_sync();
		performance_counters.sync_time.stop_timer();
		performance_counters.task_time.start_timer();
	}
}

/**
 * translate a number of threads to a level in the CPU hierarchy
 */
template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_level_for_num_threads(procs_t num_threads) {
	assert(num_threads > 0);

	if(num_threads > levels[0].total_size) {
		return 0;
	}
	procs_t candidate = level_map[find_last_bit_set(num_threads - 1)];
	// With perfect binary trees (e.g. if number of processors is a power of two) this loop is never executed
	// On sane hierarchies, this loop is executed at most once
	while(levels[candidate].total_size < num_threads) {
		--candidate;
	}
	return candidate;
}

template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::is_coordinator() {
	return team_info->coordinator_id == team_info->local_id;
}

template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_local_id() {
	return team_info->local_id;
}

template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_coordinator_id() {
	return team_info->coordinator_id;
}

template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_global_id() {
	return levels[0].local_id;
}

template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_team_size() {
	return team_info->team_size;
}

template <class Scheduler, template <typename T> class StealingDeque>
procs_t BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_max_team_size() {
	return levels[0].total_size;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::start_finish_region() {
	if(is_coordinator()) {
		performance_counters.num_finishes.incr();

		// Perform cleanup on left side of finish stack
		empty_finish_stack();

		assert(finish_stack_filled_left < finish_stack_filled_right);
		--finish_stack_filled_right;

		finish_stack[finish_stack_filled_right].num_finished_remote = 0;
		finish_stack[finish_stack_filled_right].num_spawned = 0;
		// Parent is not finished when this element is finished, so leave parent empty
		finish_stack[finish_stack_filled_right].parent = NULL;
		if(current_team_task != NULL) {
			finish_stack[finish_stack_filled_right].prev_el = current_team_task->parent;
			current_team_task->parent = &(finish_stack[finish_stack_filled_right]);
		}
#ifndef NDEBUG
		else {
			// Some additional safety which may ease debugging
			finish_stack[finish_stack_filled_right].prev_el = NULL;
		}
#endif
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::end_finish_region() {
	performance_counters.task_time.stop_timer();
	if(is_coordinator()) {
		assert(current_team_task->parent == &(finish_stack[finish_stack_filled_right]));
		// Store current team task
		TeamTaskData* my_team_task = current_team_task;

		if(finish_stack[finish_stack_filled_right].num_spawned > finish_stack[finish_stack_filled_right].num_finished_remote) {
			// There exist some non-executed or stolen tasks

			performance_counters.queue_processing_time.start_timer();
			// Execute the rest of the tasks in the team
			bool fin = coordinate_team_until_finished(&(finish_stack[finish_stack_filled_right]));
			performance_counters.queue_processing_time.stop_timer();

			if(!fin) {
				// Process other tasks until this task has been finished
				wait_for_finish(&(finish_stack[finish_stack_filled_right]));
			}
		}
		if(my_team_task != NULL)
			my_team_task->parent = finish_stack[finish_stack_filled_right].prev_el;

		if(current_team == NULL || current_team->level != my_team_task->team_level) {
			create_team(my_team_task->team_size);
			if(my_team_task->team_size > 1) {
				announce_first_team_task(my_team_task);
			}
		}
		else if(my_team_task->team_size > 1) {
			announce_next_team_task(my_team_task);
		}

		// Remove stack element
		++finish_stack_filled_right;
	}
	else {
		wait_for_coordinator_finish(current_team_task);
	}
	performance_counters.task_time.start_timer();
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::spawn(TaskParams&& ... params) {
	if(is_coordinator()) {
		performance_counters.num_spawns.incr();
		if(team_info->team_size == 1) {
			// Special treatment of solo tasks - less synchronization needed for switch to synchroneous calls
			procs_t level = team_info->team_level;
			if(stealing_deques[level]->get_length() >= levels[level].spawn_same_size_threshold) {
				performance_counters.num_spawns_to_call.incr();
				// There are enough tasks in our queue - make a synchroneous call instead
				call<CallTaskType>(static_cast<TaskParams&&>(params) ...);
			}
			else {
				CallTaskType* task = new CallTaskType(static_cast<TaskParams&&>(params) ...);
				assert(current_team_task->parent != NULL);
				current_team_task->parent->num_spawned++;
				DequeItem di;
				di.task = task;
				di.finish_stack_element = current_team_task->parent;
				di.team_size = team_info->team_size;
				store_item_in_deque(di, level);
			}
		}
		else {
			// TODO: optimization to use call in some cases to prevent the deque from growing too large
			CallTaskType* task = new CallTaskType(static_cast<TaskParams&&>(params) ...);
			assert(current_team_task->parent != NULL);
			current_team_task->parent->num_spawned++;
			DequeItem di;
			di.task = task;
			di.finish_stack_element = current_team_task->parent;
			di.team_size = team_info->team_size;
			store_item_in_deque(di, team_info->team_level);
		}
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::local_spawn(TaskParams&& ... params) {
	if(team_info->team_size == 1) {
		// Use the optimized synchroneous version instead
		spawn<CallTaskType, TaskParams ...>(static_cast<TaskParams&&>(params) ...);
	}
	else {
		performance_counters.num_spawns.incr();

		// We are out of sync, so calls instead of spawns are not possible
		CallTaskType* task = new CallTaskType(static_cast<TaskParams&&>(params) ...);
		assert(finish_stack_filled_left > 0);
		current_team_task->parent->num_spawned++;
		DequeItem di;
		di.task = task;
		di.finish_stack_element = current_team_task->parent;
		di.team_size = current_team->team_size;
		store_item_in_deque(di, team_info->team_level);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call(TaskParams&& ... params) {
	if(is_coordinator()) {
		performance_counters.num_calls.incr();
		performance_counters.num_tasks_at_level.incr(team_info->team_level);

		if(team_info->team_size > 1) {
			// TODO
			assert(true);
		}
		else {
			CallTaskType task(static_cast<TaskParams&&>(params) ...);
			task(*this);
		}
	}
	else {
		// TODO
		assert(true);
	}

	/*
	if(is_coordinator) {
		assert(finish_stack_filled_left > 0);

		if(team_size == 1) {
			CallTaskType task(params ...);
			call_team_task(&task);
		}
		else {
			CallTaskType* task = new CallTaskType(params ...);
			call_task(team_size, task, &(finish_stack[finish_stack_filled_left - 1]));
		}
	}
	else {
		join_coordinator_subteam();
	}*/
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish(TaskParams&& ... params) {
	start_finish_region();

	call<CallTaskType>(static_cast<TaskParams&&>(params) ...);

	end_finish_region();

/*	if(is_coordinator()) {
		assert(finish_stack_filled_left > 0);

		if(team_size == 1) {
			// Create task
			CallTaskType task(params ...);

			// Create a new finish_stack element for new task
			// num_finished_remote is not required as this finish_stack element blocks lower ones from finishing anyway
			finish_team_task(&task, NULL);
		}
		else {
			CallTaskType* task = new CallTaskType(params ...);

			// Create a new finish_stack element for new task
			// num_finished_remote is not required as this finish_stack element blocks lower ones from finishing anyway
			finish_task(team_size, task, NULL);
		}
	}
	else {
		join_coordinator_subteam();
	}*/
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::spawn_nt(procs_t nt_size, TaskParams&& ... params) {
	procs_t level = get_level_for_num_threads(nt_size);
	if(level == team_info->team_level) {
		// If we stay at the same level use the optimized method for same-size spawns
		spawn<CallTaskType, TaskParams ...>(static_cast<TaskParams&&>(params) ...);
	}
	else {
		// TODO: optimization to use call in some cases to prevent the finish_stack from growing too large

		// TODO: let tasks be spawned by multiple threads

		if(is_coordinator()) {
			performance_counters.num_spawns.incr();

			CallTaskType* task = new CallTaskType(static_cast<TaskParams&&>(params) ...);
			assert(current_team_task->parent != NULL);
			current_team_task->parent->num_spawned++;
			DequeItem di;
			di.task = task;
			di.finish_stack_element = current_team_task->parent;
			di.team_size = levels[level].total_size;
			store_item_in_deque(di, level);
		}
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::local_spawn_nt(procs_t nt_size, TaskParams&& ... params) {
	procs_t level = get_level_for_num_threads(nt_size);
	if(level == team_info->team_level) {
		// Use the optimized synchroneous version instead
		spawn_nt<CallTaskType, TaskParams ...>(nt_size, static_cast<TaskParams&&>(params) ...);
	}
	else {
		CallTaskType* task = new CallTaskType(static_cast<TaskParams&&>(params) ...);
		assert(finish_stack_filled_left > 0);
		current_team_task->parent.num_spawned++;
		DequeItem di;
		di.task = task;
		di.finish_stack_element = current_team_task->parent;
		di.team_size = levels[level].total_size;
		store_item_in_deque(di, level);
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::call_nt(procs_t nt_size, TaskParams&& ... params) {
	// TODO
	assert(true);
	/*
	if(is_coordinator()) {
		assert(finish_stack_filled_left > 0);

		if(team_size == 1) {
			CallTaskType task(params ...);
			call_team_task(&task);
		}
		else {
			CallTaskType* task = new CallTaskType(params ...);
			call_task(team_size, task, &(finish_stack[finish_stack_filled_left - 1]));
		}
	}
	else {
		join_coordinator_subteam(nt_size);
	}*/
}

template <class Scheduler, template <typename T> class StealingDeque>
template<class CallTaskType, typename ... TaskParams>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::finish_nt(procs_t nt_size, TaskParams&& ... params) {
	start_finish_region();

	call_nt<CallTaskType>(nt_size, static_cast<TaskParams&&>(params) ...);

	end_finish_region();

		/*
	if(is_coordinator()) {
		assert(finish_stack_filled_left > 0);

		if(nt_size == 1) {
			// Create task
			CallTaskType task(params ...);

			// Create a new finish_stack element for new task
			// num_finished_remote is not required as this finish_stack element blocks lower ones from finishing anyway
			finish_team_task(&task, NULL);
		}
		else {
			CallTaskType* task = new CallTaskType(params ...);

			// Create a new finish_stack element for new task
			// num_finished_remote is not required as this finish_stack element blocks lower ones from finishing anyway
			finish_task(nt_size, task, NULL);
		}
	}
	else {
		join_coordinator_subteam(nt_size);
	}*/
}

/*
 * Checks whether there is still some local work that we can execute
 */
template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::has_local_work() {
	while(lowest_level_deque != NULL) {
		if(!(*lowest_level_deque)->is_empty()) {
			return true;
		}
		if(lowest_level_deque >= highest_level_deque) {
			lowest_level_deque = NULL;
			highest_level_deque = NULL;
			return false;
		}
		++lowest_level_deque;
	}
	return false;
}

/*
 * Checks whether there is still some local work that we can execute
 */
template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::has_local_work(procs_t min_level) {
	StealingDeque<DequeItem>** limit_deque = stealing_deques + min_level;
	while(highest_level_deque != NULL && highest_level_deque >= limit_deque) {
		if(!(*highest_level_deque)->is_empty()) {
			return true;
		}
		if(lowest_level_deque >= highest_level_deque) {
			lowest_level_deque = NULL;
			highest_level_deque = NULL;
			return false;
		}
		--highest_level_deque;
	}
	return false;
}

/*
 * Get a task from the local queues that is suited for the current team
 */
template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::DequeItem
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_next_team_task() {
	return (*(stealing_deques + current_team->level))->pop();
/*	DequeItem ret;
	while((ret = (*current_deque)->pop()).task == NULL) {
		if(current_deque == lowest_level_deque) {
			current_deque = NULL;
			return ret;
		}
		++current_deque;
	}
	return ret;*/
}

/*
 * Get any task from the local queues
 */
template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::DequeItem
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_next_queue_task() {
	DequeItem ret;
	while((ret = (*highest_level_deque)->pop()).task == NULL) {
		if(highest_level_deque == lowest_level_deque) {
			lowest_level_deque = NULL;
			highest_level_deque = NULL;
			return ret;
		}
		--highest_level_deque;
	}
//	current_deque = highest_level_deque;
	return ret;
}

/*
 * Get any task from the local queues
 */
template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::DequeItem
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_next_queue_task(procs_t min_level) {
	StealingDeque<DequeItem>** limit_deque = stealing_deques + min_level;

	DequeItem ret;
	while((ret = (*highest_level_deque)->pop()).task == NULL) {
		if(highest_level_deque == lowest_level_deque) {
			lowest_level_deque = NULL;
			highest_level_deque = NULL;
			return ret;
		}
		--highest_level_deque;
		if(highest_level_deque < limit_deque) {
			return nullable_traits<DequeItem>::null_value;
		}
	}
	assert(ret.team_size <= this->levels[min_level].total_size);
//	current_deque = highest_level_deque;
	return ret;
}

/*
 * Steal tasks from the given partner, but only those where the level is larger than the given level
 * The current implementation makes some reasonable assumptions about the CPU hierarchy (expects some symmetry)
 * If those assumptions are not met, tasks might sometimes be executed with less threads than requested.
 * Other than that, it shouldn't create any other problems.
 */
template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::DequeItem
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::steal_tasks_from_partner(TaskExecutionContext* partner, procs_t min_level) {
	performance_counters.num_steal_calls.incr();
	performance_counters.num_steal_calls_per_thread.incr(get_global_id());

	if(partner->highest_level_deque == NULL) {
		performance_counters.num_unsuccessful_steal_calls.incr();
		performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
		return nullable_traits<DequeItem>::null_value;
	}

	procs_t partner_level = partner->highest_level_deque - partner->stealing_deques;
	procs_t my_level = partner_level;

	if(num_levels > partner->num_levels) {
		my_level += num_levels - partner->num_levels;
	}

	while(my_level >= min_level) {
		while(this->levels[my_level].total_size < partner->levels[partner_level].total_size) {
			--my_level;
		}

		if(my_level < min_level) {
			break;
		}

		StealingDeque<DequeItem>* partner_queue = partner->stealing_deques[partner_level];

		if(!partner_queue->is_empty()) {
			if(this->levels[my_level].total_size == partner->levels[partner_level].total_size) {
				// The easy case - we can steal_push tasks
				StealingDeque<DequeItem>* my_queue = stealing_deques[my_level];

				// try steal
				DequeItem ret = partner_queue->steal_push(*my_queue);
				if(ret.task != NULL) {
					assert(highest_level_deque == NULL || highest_level_deque < stealing_deques + my_level);
				//	current_deque = my_queue;
					highest_level_deque = stealing_deques + my_level;
					if(lowest_level_deque == NULL) {
						lowest_level_deque = highest_level_deque;
					}
					return ret;
				}
			}
			else {
				// We have to be more careful here - no steal_push or we might end up having the wrong tasks in the deque
				DequeItem ret = partner_queue->steal();
				if(ret.task != NULL) {
					return ret;
				}
			}
		}
		--my_level;
		--partner_level;
	}
	/*
	StealingDeque<DequeItem>** my_queue;
	StealingDeque<DequeItem>** partner_queue;
	StealingDeque<DequeItem>** partner_min;

	if(partner->num_levels > num_levels) {
		min_level += partner->num_levels - num_levels;

		assert(min_level < partner->num_levels);
	}
	partner_min = partner->stealing_deques + min_level;

	partner_queue = partner->highest_level_deque;
	if(partner_queue < partner_min || partner_queue == NULL / * actually not really necessary, but just in case NULL is not 0... *//*) {
		performance_counters.num_unsuccessful_steal_calls.incr();
		performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
		return nullable_traits<DequeItem>::null_value;
	}
	assert(partner_queue >= partner->stealing_deques && partner_queue < partner->stealing_deques + partner->num_levels);
	my_queue = stealing_deques + num_levels - (partner->stealing_deques + partner->num_levels - partner_queue);
	while(partner_queue >= partner_min) {
		if(!(*partner_queue)->is_empty()) {
			DequeItem ret = (*partner_queue)->steal_push(**my_queue);
			if(ret.task != NULL) {
				assert(highest_level_deque == NULL || highest_level_deque < my_queue);
			//	current_deque = my_queue;
				highest_level_deque = my_queue;
				if(lowest_level_deque == NULL) {
					lowest_level_deque = my_queue;
				}
			}
			else {
				performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
				performance_counters.num_unsuccessful_steal_calls.incr();
			}
			return ret;
		}
		--partner_queue;
		--my_queue;
	}*/

	performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
	performance_counters.num_unsuccessful_steal_calls.incr();
	return nullable_traits<DequeItem>::null_value;
}

/*
 * Steal tasks from the given partner, but only those where the level is larger than the given level
 * The current implementation makes some reasonable assumptions about the CPU hierarchy (expects some symmetry)
 * If those assumptions are not met, tasks might sometimes be executed with less threads than requested.
 * Other than that, it shouldn't create any other problems.
 */
template <class Scheduler, template <typename T> class StealingDeque>
typename BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::DequeItem
BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::steal_for_sync(TeamAnnouncement* my_team, TaskExecutionContext* partner, procs_t min_level) {
	performance_counters.num_steal_calls.incr();
	performance_counters.num_steal_calls_per_thread.incr(get_global_id());

	if(partner->highest_level_deque == NULL) {
		performance_counters.num_unsuccessful_steal_calls.incr();
		performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
		return nullable_traits<DequeItem>::null_value;
	}

	procs_t partner_level = partner->highest_level_deque - partner->stealing_deques;
	procs_t my_level = partner_level;

	if(num_levels > partner->num_levels) {
		my_level += num_levels - partner->num_levels;
	}

	while(my_level >= min_level) {
		while(this->levels[my_level].total_size < partner->levels[partner_level].total_size) {
			--my_level;
		}

		if(my_level < min_level) {
			break;
		}

		StealingDeque<DequeItem>* partner_queue = partner->stealing_deques[partner_level];

		if(!partner_queue->is_empty()) {
			// try to deregister before stealing
			if(!deregister_from_team(my_team)) {
				performance_counters.num_unsuccessful_steal_calls.incr();
				performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
				return nullable_traits<DequeItem>::null_value;
			}

			if(this->levels[my_level].total_size == partner->levels[partner_level].total_size) {
				// The easy case - we can steal_push tasks
				StealingDeque<DequeItem>* my_queue = stealing_deques[my_level];

				// try steal
				DequeItem ret = partner_queue->steal_push(*my_queue);
				if(ret.task != NULL) {
					assert(highest_level_deque == NULL || highest_level_deque < stealing_deques + my_level);
				//	current_deque = my_queue;
					highest_level_deque = stealing_deques + my_level;
					if(lowest_level_deque == NULL) {
						lowest_level_deque = highest_level_deque;
					}
					return ret;
				}
			}
			else {
				// We have to be more careful here - no steal_push or we might end up having the wrong tasks in the deque
				DequeItem ret = partner_queue->steal();
				if(ret.task != NULL) {
					return ret;
				}
			}
			// stealing failed - reregister and continue
			register_for_team(my_team);
		}
		--my_level;
		--partner_level;
	}

/*
	if(partner->num_levels > num_levels) {
		min_level += partner->num_levels - num_levels;

		assert(min_level < partner->num_levels);
	}
	partner_min = partner->stealing_deques + min_level;

	partner_queue = partner->highest_level_deque;
	if(partner_queue < partner_min || partner_queue == NULL / * actually not really necessary, but just in case NULL is not 0... *//*) {
		performance_counters.num_unsuccessful_steal_calls.incr();
		performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
		return nullable_traits<DequeItem>::null_value;
	}
	assert(partner_queue >= partner->stealing_deques && partner_queue < partner->stealing_deques + partner->num_levels);
	my_queue = stealing_deques + num_levels - (partner->stealing_deques + partner->num_levels - partner_queue);
	while(partner_queue >= partner_min) {
		if(!(*partner_queue)->is_empty()) {
			// try to deregister before stealing
			if(!deregister_from_team(my_team)) {
				performance_counters.num_unsuccessful_steal_calls.incr();
				performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
				return nullable_traits<DequeItem>::null_value;
			}

			// try steal
			DequeItem ret = (*partner_queue)->steal_push(**my_queue);
			if(ret.task != NULL) {
				assert(highest_level_deque == NULL || highest_level_deque < my_queue);
			//	current_deque = my_queue;
				highest_level_deque = my_queue;
				if(lowest_level_deque == NULL) {
					lowest_level_deque = my_queue;
				}
				return ret;
			}
			// stealing failed - reregister and continue
			register_for_team(my_team);
		}
		--partner_queue;
		--my_queue;
	}*/

	performance_counters.num_unsuccessful_steal_calls.incr();
	performance_counters.num_unsuccessful_steal_calls_per_thread.incr(get_global_id());
	return nullable_traits<DequeItem>::null_value;
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::store_item_in_deque(DequeItem di, procs_t level) {
	assert(di.team_size <= this->levels[level].total_size);
	stealing_deques[level]->push(di);
	if(lowest_level_deque == NULL) {
		lowest_level_deque = stealing_deques + level;
		highest_level_deque = lowest_level_deque;
	}
	else if(lowest_level_deque > stealing_deques + level) {
		lowest_level_deque = stealing_deques + level;
	}
	else if(highest_level_deque < stealing_deques + level) {
		highest_level_deque = stealing_deques + level;
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
void BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::register_for_team(TeamAnnouncement* team) {
	current_team = team;

	// Registration
	Registration reg, old_reg;
	Backoff bo;
	reg.complete = team->reg.complete;
	old_reg.complete = reg.complete;
	assert(reg.parts.a != reg.parts.r);
	++reg.parts.a;
	while(!UINT64_CAS(&(team->reg.complete), old_reg.complete, reg.complete)) {
		bo.backoff();
		reg.complete = team->reg.complete;
		old_reg.complete = reg.complete;
		++reg.parts.a;
	}
}

template <class Scheduler, template <typename T> class StealingDeque>
bool BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::deregister_from_team(TeamAnnouncement* team) {
	// Deregistration
	Registration reg, old_reg;
	Backoff bo;
	reg.complete = team->reg.complete;
	old_reg.complete = reg.complete;
	if(reg.parts.a == reg.parts.r) {
		return false;
	}
	assert(reg.parts.a != 0);
	--reg.parts.a;
	while(!UINT64_CAS(&(team->reg.complete), old_reg.complete, reg.complete)) {
		bo.backoff();
		reg.complete = team->reg.complete;
		if(reg.parts.a == reg.parts.r) {
			return false;
		}
		old_reg.complete = reg.complete;
		assert(reg.parts.a != 0);
		--reg.parts.a;
	}

	current_team = NULL;
	return true;
}

template <class Scheduler, template <typename T> class StealingDeque>
boost::mt19937& BasicMixedModeSchedulerTaskExecutionContext<Scheduler, StealingDeque>::get_rng() {
	return rng;
}

}

#endif /* BASICMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_ */
