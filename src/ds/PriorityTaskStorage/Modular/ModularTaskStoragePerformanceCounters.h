/*
 * ModularTaskStoragePerformanceCounters.h
 *
 *  Created on: Oct 28, 2011
 *      Author: mwimmer
 */

#ifndef MODULARTASKSTORAGEPERFORMANCECOUNTERS_H_
#define MODULARTASKSTORAGEPERFORMANCECOUNTERS_H_

#include "../../../settings.h"

#include "../../../primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"
#include "../../../primitives/PerformanceCounter/Time/TimePerformanceCounter.h"

namespace pheet {

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
class ModularTaskStoragePerformanceCounters {
public:
	ModularTaskStoragePerformanceCounters();
	ModularTaskStoragePerformanceCounters(ModularTaskStoragePerformanceCounters& other);
	~ModularTaskStoragePerformanceCounters();

	void print_headers();
	void print_values();

//private:
	PrimaryPerformanceCounters primary_perf_count;
	SecondaryPerformanceCounters secondary_perf_count;
};

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::ModularTaskStoragePerformanceCounters() {

}

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::ModularTaskStoragePerformanceCounters(ModularTaskStoragePerformanceCounters& other)
:primary_perf_count(other.primary_perf_count),
 secondary_perf_count(other.secondary_perf_count)
{

}

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::~ModularTaskStoragePerformanceCounters() {
}

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline void ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::print_headers() {
	primary_perf_count.print_headers();
	secondary_perf_count.print_headers();
}

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline void ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::print_values() {
	primary_perf_count.print_values();
	secondary_perf_count.print_values();
}

}

#endif /* MODULARTASKSTORAGEPERFORMANCECOUNTERS_H_ */