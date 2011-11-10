/*
 * PrioritySchedulerPerformanceCounters.h
 *
 *  Created on: Oct 28, 2011
 *      Author: mwimmer
 */

#ifndef PRIORITYSCHEDULERPERFORMANCECOUNTERS_H_
#define PRIORITYSCHEDULERPERFORMANCECOUNTERS_H_

#include "../../../settings.h"

#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Max/MaxPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Min/MinPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Time/TimePerformanceCounter.h"

namespace pheet {

template <class TaskStoragePerformanceCounters>
class PrioritySchedulerPerformanceCounters {
public:
	PrioritySchedulerPerformanceCounters() {}
	PrioritySchedulerPerformanceCounters(PrioritySchedulerPerformanceCounters<TaskStoragePerformanceCounters>& other)
		: num_spawns(other.num_spawns), num_spawns_to_call(other.num_spawns_to_call),
		  num_calls(other.num_calls), num_finishes(other.num_finishes),
		  num_steals(other.num_steals), num_steal_calls(other.num_steal_calls),
		  num_unsuccessful_steal_calls(other.num_unsuccessful_steal_calls),
		  num_stealing_deque_pop_cas(other.num_stealing_deque_pop_cas),
		  total_time(other.total_time), task_time(other.task_time),
		  idle_time(other.idle_time),
		  finish_stack_nonblocking_max(other.finish_stack_nonblocking_max),
		  finish_stack_blocking_min(other.finish_stack_blocking_min),
		  task_storage_performance_counters(other.task_storage_performance_counters) {}

	void print_headers();
	void print_values();

//private:
	BasicPerformanceCounter<scheduler_count_spawns> num_spawns;
	BasicPerformanceCounter<scheduler_count_spawns_to_call> num_spawns_to_call;
	BasicPerformanceCounter<scheduler_count_calls> num_calls;
	BasicPerformanceCounter<scheduler_count_finishes> num_finishes;

	BasicPerformanceCounter<stealing_deque_count_steal_calls> num_steal_calls;
	BasicPerformanceCounter<stealing_deque_count_unsuccessful_steal_calls> num_unsuccessful_steal_calls;

	TimePerformanceCounter<scheduler_measure_total_time> total_time;
	TimePerformanceCounter<scheduler_measure_task_time> task_time;
	TimePerformanceCounter<scheduler_measure_idle_time> idle_time;

	MaxPerformanceCounter<size_t, scheduler_measure_finish_stack_nonblocking_max> finish_stack_nonblocking_max;
	MinPerformanceCounter<size_t, scheduler_measure_finish_stack_blocking_min> finish_stack_blocking_min;

	TaskStoragePerformanceCounters task_storage_performance_counters;
};

template <class TaskStoragePerformanceCounters>
inline void PrioritySchedulerPerformanceCounters<TaskStoragePerformanceCounters>::print_headers() {
	BasicPerformanceCounter<scheduler_count_spawns>::print_header("spawns\t");
	BasicPerformanceCounter<scheduler_count_calls>::print_header("calls\t");
	BasicPerformanceCounter<scheduler_count_spawns_to_call>::print_header("spawns->call\t");
	BasicPerformanceCounter<scheduler_count_finishes>::print_header("finishes\t");

	BasicPerformanceCounter<stealing_deque_count_steal_calls>::print_header("steal_calls\t");
	BasicPerformanceCounter<stealing_deque_count_unsuccessful_steal_calls>::print_header("unsuccessful_steal_calls\t");

	TimePerformanceCounter<scheduler_measure_total_time>::print_header("scheduler_total_time\t");
	TimePerformanceCounter<scheduler_measure_task_time>::print_header("total_task_time\t");
	TimePerformanceCounter<scheduler_measure_idle_time>::print_header("total_idle_time\t");

	MaxPerformanceCounter<size_t, scheduler_measure_finish_stack_nonblocking_max>::print_header("finish_stack_nonblocking_max\t");
	MinPerformanceCounter<size_t, scheduler_measure_finish_stack_blocking_min>::print_header("finish_stack_blocking_min\t");

	task_storage_performance_counters.print_headers();
}

template <class TaskStoragePerformanceCounters>
inline void PrioritySchedulerPerformanceCounters<TaskStoragePerformanceCounters>::print_values() {
	num_spawns.print("%d\t");
	num_calls.print("%d\t");
	num_spawns_to_call.print("%d\t");
	num_finishes.print("%d\t");
	num_steal_calls.print("%d\t");
	num_unsuccessful_steal_calls.print("%d\t");
	total_time.print("%f\t");
	task_time.print("%f\t");
	idle_time.print("%f\t");

	finish_stack_nonblocking_max.print("%d\t");
	finish_stack_blocking_min.print("%d\t");

	task_storage_performance_counters.print_values();
}

}

#endif /* PRIORITYSCHEDULERPERFORMANCECOUNTERS_H_ */
