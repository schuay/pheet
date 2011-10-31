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
	BasicPerformanceCounter<task_storage_count_steals> num_stolen;
	BasicPerformanceCounter<task_storage_count_unsuccessful_pops> num_unsuccessful_pops;
	BasicPerformanceCounter<task_storage_count_successful_pops> num_successful_pops;
	BasicPerformanceCounter<task_storage_count_unsuccessful_steals> num_unsuccessful_steals;
	BasicPerformanceCounter<task_storage_count_successful_steals> num_successful_steals;
	BasicPerformanceCounter<task_storage_count_size_pop> total_size_pop;
	BasicPerformanceCounter<task_storage_count_size_steal> total_size_steal;
	TimePerformanceCounter<task_storage_measure_pop_time> pop_time;
	TimePerformanceCounter<task_storage_measure_steal_time> steal_time;

	PrimaryPerformanceCounters primary_perf_count;
	SecondaryPerformanceCounters secondary_perf_count;
};

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::ModularTaskStoragePerformanceCounters() {

}

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::ModularTaskStoragePerformanceCounters(ModularTaskStoragePerformanceCounters& other)
:num_stolen(other.num_stolen),
 num_unsuccessful_pops(other.num_unsuccessful_pops),
 num_successful_pops(other.num_successful_pops),
 num_unsuccessful_steals(other.num_unsuccessful_steals),
 num_successful_steals(other.num_successful_steals),
 total_size_pop(other.total_size_pop),
 total_size_steal(other.total_size_steal),
 pop_time(other.pop_time),
 steal_time(other.steal_time),
 primary_perf_count(other.primary_perf_count),
 secondary_perf_count(other.secondary_perf_count)
{

}

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::~ModularTaskStoragePerformanceCounters() {
}

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline void ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::print_headers() {
	BasicPerformanceCounter<task_storage_count_steals>::print_header("num_stolen\t");
	BasicPerformanceCounter<task_storage_count_unsuccessful_pops>::print_header("num_unsuccessful_pops\t");
	BasicPerformanceCounter<task_storage_count_successful_pops>::print_header("num_successful_pops\t");
	BasicPerformanceCounter<task_storage_count_unsuccessful_steals>::print_header("num_unsuccessful_steals\t");
	BasicPerformanceCounter<task_storage_count_successful_steals>::print_header("num_successful_steals\t");
	BasicPerformanceCounter<task_storage_count_size_pop>::print_header("total_size_pop\t");
	BasicPerformanceCounter<task_storage_count_size_steal>::print_header("total_size_steal\t");
	TimePerformanceCounter<task_storage_measure_pop_time>::print_header("pop_time\t");
	TimePerformanceCounter<task_storage_measure_steal_time>::print_header("steal_time\t");

	primary_perf_count.print_headers();
	secondary_perf_count.print_headers();
}

template <class PrimaryPerformanceCounters, class SecondaryPerformanceCounters>
inline void ModularTaskStoragePerformanceCounters<PrimaryPerformanceCounters, SecondaryPerformanceCounters>::print_values() {
	num_stolen.print("%d\t");
	num_unsuccessful_pops.print("%d\t");
	num_successful_pops.print("%d\t");
	num_unsuccessful_steals.print("%d\t");
	num_successful_steals.print("%d\t");
	total_size_pop.print("%d\t");
	total_size_steal.print("%d\t");
	pop_time.print("%f\t");
	steal_time.print("%f\t");

	primary_perf_count.print_values();
	secondary_perf_count.print_values();
}

}

#endif /* MODULARTASKSTORAGEPERFORMANCECOUNTERS_H_ */
