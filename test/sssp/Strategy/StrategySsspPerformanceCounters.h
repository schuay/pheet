/*
 * StrategySsspPerformanceCounters.h
 *
 *  Created on: Jan 31, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef STRATEGYSSSPPERFORMANCECOUNTERS_H_
#define STRATEGYSSSPPERFORMANCECOUNTERS_H_

#include <pheet/primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h>
#include <pheet/primitives/PerformanceCounter/Time/LastTimePerformanceCounter.h>

namespace pheet {

template <class Pheet>
class StrategySsspPerformanceCounters {
public:
	typedef StrategySsspPerformanceCounters<Pheet> Self;

	StrategySsspPerformanceCounters() {}
	StrategySsspPerformanceCounters(Self& other)
	:num_dead_tasks(other.num_dead_tasks),
	 num_actual_tasks(other.num_actual_tasks),
	 last_non_dead_time(other.last_non_dead_time),
	 last_task_time(other.last_task_time),
	 last_update_time(other.last_update_time){}
	StrategySsspPerformanceCounters(Self&& other)
	:num_dead_tasks(other.num_dead_tasks),
	 num_actual_tasks(other.num_actual_tasks),
	 last_non_dead_time(other.last_non_dead_time),
	 last_task_time(other.last_task_time),
	 last_update_time(other.last_update_time) {}
	~StrategySsspPerformanceCounters() {}

	static void print_headers() {
		BasicPerformanceCounter<Pheet, sssp_count_dead_tasks>::print_header("num_dead_tasks\t");
		BasicPerformanceCounter<Pheet, sssp_count_actual_tasks>::print_header("num_actual_tasks\t");
	}
	void print_values() {
		num_dead_tasks.print("%d\t");
		num_actual_tasks.print("%d\t");
	}

	BasicPerformanceCounter<Pheet, sssp_count_dead_tasks> num_dead_tasks;
	BasicPerformanceCounter<Pheet, sssp_count_actual_tasks> num_actual_tasks;
	LastTimePerformanceCounter<Pheet, sssp_measure_last_non_dead_time> last_non_dead_time;
	LastTimePerformanceCounter<Pheet, sssp_measure_last_task_time> last_task_time;
	LastTimePerformanceCounter<Pheet, sssp_measure_last_update_time> last_update_time;
};

} /* namespace context */
#endif /* STRATEGYSSSPPERFORMANCECOUNTERS_H_ */
