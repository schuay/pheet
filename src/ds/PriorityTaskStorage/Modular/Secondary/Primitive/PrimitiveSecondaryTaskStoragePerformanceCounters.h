/*
 * PrimitiveSecondaryTaskStoragePerformanceCounters.h
 *
 *  Created on: Nov 8, 2011
 *      Author: mwimmer
 */

#ifndef PRIMITIVESECONDARYTASKSTORAGEPERFORMANCECOUNTERS_H_
#define PRIMITIVESECONDARYTASKSTORAGEPERFORMANCECOUNTERS_H_

#include "../../../../../settings.h"

namespace pheet {

class PrimitiveSecondaryTaskStoragePerformanceCounters {
public:
	PrimitiveSecondaryTaskStoragePerformanceCounters();
	PrimitiveSecondaryTaskStoragePerformanceCounters(PrimitiveSecondaryTaskStoragePerformanceCounters& other);
	~PrimitiveSecondaryTaskStoragePerformanceCounters();

	void print_headers();
	void print_values();

	BasicPerformanceCounter<task_storage_count_steals> num_stolen;
	BasicPerformanceCounter<task_storage_count_unsuccessful_steals> num_unsuccessful_steals;
	BasicPerformanceCounter<task_storage_count_successful_steals> num_successful_steals;
	BasicPerformanceCounter<task_storage_count_size_steal> total_size_steal;
	TimePerformanceCounter<task_storage_measure_steal_time> steal_time;
};

inline PrimitiveSecondaryTaskStoragePerformanceCounters::PrimitiveSecondaryTaskStoragePerformanceCounters() {

}

inline PrimitiveSecondaryTaskStoragePerformanceCounters::PrimitiveSecondaryTaskStoragePerformanceCounters(PrimitiveSecondaryTaskStoragePerformanceCounters& other)
:num_stolen(other.num_stolen),
 num_unsuccessful_steals(other.num_unsuccessful_steals),
 num_successful_steals(other.num_successful_steals),
 total_size_steal(other.total_size_steal),
 steal_time(other.steal_time)
{

}

inline PrimitiveSecondaryTaskStoragePerformanceCounters::~PrimitiveSecondaryTaskStoragePerformanceCounters() {

}

inline void PrimitiveSecondaryTaskStoragePerformanceCounters::print_headers() {
	BasicPerformanceCounter<task_storage_count_steals>::print_header("num_stolen\t");
	BasicPerformanceCounter<task_storage_count_unsuccessful_steals>::print_header("num_unsuccessful_steals\t");
	BasicPerformanceCounter<task_storage_count_successful_steals>::print_header("num_successful_steals\t");
	BasicPerformanceCounter<task_storage_count_size_steal>::print_header("total_size_steal\t");
	TimePerformanceCounter<task_storage_measure_steal_time>::print_header("steal_time\t");
}

inline void PrimitiveSecondaryTaskStoragePerformanceCounters::print_values() {
	num_stolen.print("%d\t");
	num_unsuccessful_steals.print("%d\t");
	num_successful_steals.print("%d\t");
	total_size_steal.print("%d\t");
	steal_time.print("%f\t");
}

}

#endif /* PRIMITIVESECONDARYTASKSTORAGEPERFORMANCECOUNTERS_H_ */
