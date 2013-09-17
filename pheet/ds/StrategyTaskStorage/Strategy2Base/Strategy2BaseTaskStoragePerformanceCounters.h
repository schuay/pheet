/*
 * Strategy2BaseTaskStoragePerformanceCounters.h
 *
 *  Created on: Sep 17, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef STRATEGY2BASETASKSTORAGEPERFORMANCECOUNTERS_H_
#define STRATEGY2BASETASKSTORAGEPERFORMANCECOUNTERS_H_

namespace pheet {

template <class Pheet>
class Strategy2BaseTaskStoragePerformanceCounters {
public:
	typedef Strategy2BaseTaskStoragePerformanceCounters<Pheet> Self;

	Strategy2BaseTaskStoragePerformanceCounters() {}
	Strategy2BaseTaskStoragePerformanceCounters(Self& other) {}
	~Strategy2BaseTaskStoragePerformanceCounters() {}

	inline static void print_headers() {
	}

	inline void print_values() {
	}
};

} /* namespace pheet */
#endif /* STRATEGY2BASETASKSTORAGEPERFORMANCECOUNTERS_H_ */
