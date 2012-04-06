/*
 * LinkedListStrategyTaskStoragePerformanceCounters.h
 *
 *  Created on: 06.04.2012
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef LINKEDLISTSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_
#define LINKEDLISTSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_

namespace pheet {

template <class Pheet, class StrategyHeapPerformanceCounters>
class LinkedListStrategyTaskStoragePerformanceCounters {
public:
	typedef LinkedListStrategyTaskStoragePerformanceCounters<Pheet, StrategyHeapPerformanceCounters> Self;

	inline LinkedListStrategyTaskStoragePerformanceCounters() {}
	inline LinkedListStrategyTaskStoragePerformanceCounters(Self& other)
	: strategy_heap_performance_counters(other.strategy_heap_performance_counters) {}

	inline ~LinkedListStrategyTaskStoragePerformanceCounters() {}

	inline static void print_headers() {
		strategy_heap_performance_counters.print_headers();
	}
	inline void print_values() {
		strategy_heap_performance_counters.print_values();
	}

	StrategyHeapPerformanceCounters strategy_heap_performance_counters;
};

}

#endif /* LINKEDLISTSTRATEGYTASKSTORAGEPERFORMANCECOUNTERS_H_ */
