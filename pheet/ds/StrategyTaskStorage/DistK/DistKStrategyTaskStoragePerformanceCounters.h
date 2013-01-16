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
	: strategy_heap_performance_counters(other.strategy_heap_performance_counters) {}

	inline ~DistKStrategyTaskStoragePerformanceCounters() {}

	inline static void print_headers() {
		StrategyHeapPerformanceCounters::print_headers();
	}
	inline void print_values() {
		strategy_heap_performance_counters.print_values();
	}

	StrategyHeapPerformanceCounters strategy_heap_performance_counters;
};

}

#endif /* DISTKSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_ */
