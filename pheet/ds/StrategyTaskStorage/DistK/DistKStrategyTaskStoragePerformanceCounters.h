/*
 * DistKStrategyTaskStoragePerformanceCounters.h
 *
 *  Created on: 15.01.2013
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef DISTKSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_
#define DISTKSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_

namespace pheet {

template <class Pheet, class StrategyHeapPerformanceCounters>
class DistKStrategyTaskStoragePerformanceCounters {
public:
	typedef DistKStrategyTaskStoragePerformanceCounters<Pheet, StrategyHeapPerformanceCounters> Self;

	inline DistKStrategyTaskStoragePerformanceCounters() {}
	inline DistKStrategyTaskStoragePerformanceCounters(Self& other)
	:num_blocks_created(other.num_blocks_created),
	 num_global_blocks(other.num_global_blocks),
	 num_unsuccessful_takes(other.num_unsuccessful_takes),
	 num_successful_takes(other.num_successful_takes),
	 strategy_heap_performance_counters(other.strategy_heap_performance_counters) {}

	inline ~DistKStrategyTaskStoragePerformanceCounters() {}

	inline static void print_headers() {
		BasicPerformanceCounter<Pheet, task_storage_count_blocks_created>::print_header("num_blocks_created\t");
		BasicPerformanceCounter<Pheet, task_storage_count_global_blocks>::print_header("num_global_blocks\t");
		BasicPerformanceCounter<Pheet, task_storage_count_unsuccessful_takes>::print_header("num_unsuccessful_takes\t");
		BasicPerformanceCounter<Pheet, task_storage_count_successful_takes>::print_header("num_successful_takes\t");

		StrategyHeapPerformanceCounters::print_headers();
	}
	inline void print_values() {
		num_blocks_created.print("%d\t");
		num_global_blocks.print("%d\t");
		num_unsuccessful_takes.print("%d\t");
		num_successful_takes.print("%d\t");

		strategy_heap_performance_counters.print_values();
	}

	BasicPerformanceCounter<Pheet, task_storage_count_blocks_created> num_blocks_created;
	BasicPerformanceCounter<Pheet, task_storage_count_global_blocks> num_global_blocks;
	BasicPerformanceCounter<Pheet, task_storage_count_unsuccessful_takes> num_unsuccessful_takes;
	BasicPerformanceCounter<Pheet, task_storage_count_successful_takes> num_successful_takes;

	StrategyHeapPerformanceCounters strategy_heap_performance_counters;
};

}

#endif /* DISTKSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_ */
