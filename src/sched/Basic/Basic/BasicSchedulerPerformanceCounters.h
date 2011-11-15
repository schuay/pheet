/*
 * BasicSchedulerPerformanceCounters.h
 *
 *  Created on: Nov 15, 2011
 *      Author: mwimmer
 */

#ifndef BASICSCHEDULERPERFORMANCECOUNTERS_H_
#define BASICSCHEDULERPERFORMANCECOUNTERS_H_

#include "../../../settings.h"

#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Max/MaxPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Min/MinPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Time/TimePerformanceCounter.h"

namespace pheet {

class BasicSchedulerPerformanceCounters {
public:
	BasicSchedulerPerformanceCounters() {}
	BasicSchedulerPerformanceCounters(BasicSchedulerPerformanceCounters& other)
		: num_spawns(other.num_spawns), num_actual_spawns(other.num_actual_spawns),
		  num_spawns_to_call(other.num_spawns_to_call),
		  num_calls(other.num_calls), num_finishes(other.num_finishes),
		  num_completion_signals(other.num_completion_signals),
		  num_chained_completion_signals(other.num_chained_completion_signals),
		  num_non_blocking_finish_regions(other.num_non_blocking_finish_regions),
		  num_steals(other.num_steals), num_steal_calls(other.num_steal_calls),
		  num_unsuccessful_steal_calls(other.num_unsuccessful_steal_calls),
		  num_stealing_deque_pop_cas(other.num_stealing_deque_pop_cas),
		  num_dequeued_tasks(other.num_dequeued_tasks),
		  num_steal_executed_tasks(other.num_steal_executed_tasks),
		  total_time(other.total_time), task_time(other.task_time),
		  idle_time(other.idle_time) {}

	void print_headers();
	void print_values();

//private:
	BasicPerformanceCounter<scheduler_count_spawns> num_spawns;
	BasicPerformanceCounter<scheduler_count_actual_spawns> num_actual_spawns;
	BasicPerformanceCounter<scheduler_count_spawns_to_call> num_spawns_to_call;
	BasicPerformanceCounter<scheduler_count_calls> num_calls;
	BasicPerformanceCounter<scheduler_count_finishes> num_finishes;
	BasicPerformanceCounter<scheduler_count_completion_signals> num_completion_signals;
	BasicPerformanceCounter<scheduler_count_chained_completion_signals> num_chained_completion_signals;
	BasicPerformanceCounter<scheduler_count_non_blocking_finish_regions> num_non_blocking_finish_regions;

	BasicPerformanceCounter<stealing_deque_count_steals> num_steals;
	BasicPerformanceCounter<stealing_deque_count_steal_calls> num_steal_calls;
	BasicPerformanceCounter<stealing_deque_count_unsuccessful_steal_calls> num_unsuccessful_steal_calls;
	BasicPerformanceCounter<stealing_deque_count_pop_cas> num_stealing_deque_pop_cas;

	BasicPerformanceCounter<stealing_deque_count_dequeued_tasks> num_dequeued_tasks;
	BasicPerformanceCounter<stealing_deque_count_steal_executed_tasks> num_steal_executed_tasks;

	TimePerformanceCounter<scheduler_measure_total_time> total_time;
	TimePerformanceCounter<scheduler_measure_task_time> task_time;
	TimePerformanceCounter<scheduler_measure_idle_time> idle_time;
};

inline void BasicSchedulerPerformanceCounters::print_headers() {
	BasicPerformanceCounter<scheduler_count_spawns>::print_header("spawns\t");
	BasicPerformanceCounter<scheduler_count_actual_spawns>::print_header("actual_spawns\t");
	BasicPerformanceCounter<scheduler_count_spawns_to_call>::print_header("calls\t");
	BasicPerformanceCounter<scheduler_count_calls>::print_header("spawns->call\t");
	BasicPerformanceCounter<scheduler_count_finishes>::print_header("finishes\t");
	BasicPerformanceCounter<scheduler_count_completion_signals>::print_header("num_completion_signals\t");
	BasicPerformanceCounter<scheduler_count_chained_completion_signals>::print_header("num_chained_completion_signals\t");
	BasicPerformanceCounter<scheduler_count_non_blocking_finish_regions>::print_header("num_non_blocking_finish_regions\t");

	BasicPerformanceCounter<stealing_deque_count_steals>::print_header("stolen\t");
	BasicPerformanceCounter<stealing_deque_count_steal_calls>::print_header("steal_calls\t");
	BasicPerformanceCounter<stealing_deque_count_unsuccessful_steal_calls>::print_header("unsuccessful_steal_calls\t");
	BasicPerformanceCounter<stealing_deque_count_pop_cas>::print_header("stealing_deque_pop_cas\t");

	BasicPerformanceCounter<stealing_deque_count_dequeued_tasks>::print_header("num_dequeued_tasks\t");
	BasicPerformanceCounter<stealing_deque_count_steal_executed_tasks>::print_header("num_steal_executed_tasks\t");

	TimePerformanceCounter<scheduler_measure_total_time>::print_header("scheduler_total_time\t");
	TimePerformanceCounter<scheduler_measure_task_time>::print_header("total_task_time\t");
	TimePerformanceCounter<scheduler_measure_idle_time>::print_header("total_idle_time\t");
}

inline void BasicSchedulerPerformanceCounters::print_values() {
	num_spawns.print("%lu\t");
	num_actual_spawns.print("%lu\t");
	num_calls.print("%lu\t");
	num_spawns_to_call.print("%lu\t");
	num_finishes.print("%lu\t");
	num_completion_signals.print("%lu\t");
	num_chained_completion_signals.print("%lu\t");
	num_non_blocking_finish_regions.print("%lu\t");
	num_steals.print("%lu\t");
	num_steal_calls.print("%lu\t");
	num_unsuccessful_steal_calls.print("%lu\t");
	num_stealing_deque_pop_cas.print("%lu\t");
	num_dequeued_tasks.print("%lu\t");
	num_steal_executed_tasks.print("%lu\t");
	total_time.print("%f\t");
	task_time.print("%f\t");
	idle_time.print("%f\t");
}

}

#endif /* BASICSCHEDULERPERFORMANCECOUNTERS_H_ */
