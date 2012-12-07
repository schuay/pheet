/*
 * CentralKStrategyTaskStoragePerformanceCounters.h
 *
 *  Created on: 06.04.2012
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef CENTRALKSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_
#define CENTRALKSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_

namespace pheet {

template <class Pheet, class StrategyHeapPerformanceCounters>
class CentralKStrategyTaskStoragePerformanceCounters {
public:
	typedef CentralKStrategyTaskStoragePerformanceCounters<Pheet, StrategyHeapPerformanceCounters> Self;

	inline CentralKStrategyTaskStoragePerformanceCounters() {}
	inline CentralKStrategyTaskStoragePerformanceCounters(Self& other)
	: strategy_heap_performance_counters(other.strategy_heap_performance_counters) {}

	inline ~CentralKStrategyTaskStoragePerformanceCounters() {}

	inline static void print_headers() {
		StrategyHeapPerformanceCounters::print_headers();
	}
	inline void print_values() {
		strategy_heap_performance_counters.print_values();
	}

	StrategyHeapPerformanceCounters strategy_heap_performance_counters;
};

}

#endif /* CENTRALKSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_ */
