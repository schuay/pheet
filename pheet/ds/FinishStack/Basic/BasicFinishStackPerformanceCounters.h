/*
 * BasicFinishStackPerformanceCounters.h
 *
 *  Created on: Jul 30, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef BASICFINISHSTACKPERFORMANCECOUNTERS_H_
#define BASICFINISHSTACKPERFORMANCECOUNTERS_H_

#include <pheet/settings.h>

#include <pheet/primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h>
#include <pheet/primitives/PerformanceCounter/Max/MaxPerformanceCounter.h>
#include <pheet/primitives/PerformanceCounter/Min/MinPerformanceCounter.h>
#include <pheet/primitives/PerformanceCounter/Time/TimePerformanceCounter.h>

namespace pheet {

template <class Pheet>
class BasicFinishStackPerformanceCounters {
public:
	typedef BasicFinishStackPerformanceCounters<Pheet> Self;

	BasicFinishStackPerformanceCounters() {}
	BasicFinishStackPerformanceCounters(Self& other)
		: num_completion_signals(other.num_completion_signals),
		  num_chained_completion_signals(other.num_chained_completion_signals),
		  num_remote_chained_completion_signals(other.num_remote_chained_completion_signals),
		  num_non_blocking_finish_regions(other.num_non_blocking_finish_regions),
		  finish_stack_nonblocking_max(other.finish_stack_nonblocking_max),
		  finish_stack_blocking_min(other.finish_stack_blocking_min) {}

	static void print_headers();
	void print_values();

//private:
	BasicPerformanceCounter<Pheet, scheduler_count_completion_signals> num_completion_signals;
	BasicPerformanceCounter<Pheet, scheduler_count_chained_completion_signals> num_chained_completion_signals;
	BasicPerformanceCounter<Pheet, scheduler_count_remote_chained_completion_signals> num_remote_chained_completion_signals;
	BasicPerformanceCounter<Pheet, scheduler_count_non_blocking_finish_regions> num_non_blocking_finish_regions;

	MaxPerformanceCounter<Pheet, size_t, scheduler_measure_finish_stack_nonblocking_max> finish_stack_nonblocking_max;
	MinPerformanceCounter<Pheet, size_t, scheduler_measure_finish_stack_blocking_min> finish_stack_blocking_min;
};

template <class Pheet>
inline void BasicFinishStackPerformanceCounters<Pheet>::print_headers() {
	BasicPerformanceCounter<Pheet, scheduler_count_completion_signals>::print_header("num_completion_signals\t");
	BasicPerformanceCounter<Pheet, scheduler_count_chained_completion_signals>::print_header("num_chained_completion_signals\t");
	BasicPerformanceCounter<Pheet, scheduler_count_remote_chained_completion_signals>::print_header("num_remote_chained_completion_signals\t");
	BasicPerformanceCounter<Pheet, scheduler_count_non_blocking_finish_regions>::print_header("num_non_blocking_finish_regions\t");

	MaxPerformanceCounter<Pheet, size_t, scheduler_measure_finish_stack_nonblocking_max>::print_header("finish_stack_nonblocking_max\t");
	MinPerformanceCounter<Pheet, size_t, scheduler_measure_finish_stack_blocking_min>::print_header("finish_stack_blocking_min\t");
}

template <class Pheet>
inline void BasicFinishStackPerformanceCounters<Pheet>::print_values() {
	num_completion_signals.print("%lu\t");
	num_chained_completion_signals.print("%lu\t");
	num_remote_chained_completion_signals.print("%lu\t");
	num_non_blocking_finish_regions.print("%lu\t");

	finish_stack_nonblocking_max.print("%lu\t");
	finish_stack_blocking_min.print("%lu\t");
}

} /* namespace pheet */
#endif /* BASICFINISHSTACKPERFORMANCECOUNTERS_H_ */
