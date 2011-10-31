/*
 * PrimitivePrimaryTaskStoragePerformanceCounters.h
 *
 *  Created on: Oct 31, 2011
 *      Author: mwimmer
 */

#ifndef PRIMITIVEPRIMARYTASKSTORAGEPERFORMANCECOUNTERS_H_
#define PRIMITIVEPRIMARYTASKSTORAGEPERFORMANCECOUNTERS_H_

#include "../../../../../settings.h"

namespace pheet {

class PrimitivePrimaryTaskStoragePerformanceCounters {
public:
	PrimitivePrimaryTaskStoragePerformanceCounters();
	PrimitivePrimaryTaskStoragePerformanceCounters(PrimitivePrimaryTaskStoragePerformanceCounters& other);
	~PrimitivePrimaryTaskStoragePerformanceCounters();

	void print_headers();
	void print_values();

	BasicPerformanceCounter<task_storage_count_unsuccessful_pops> num_unsuccessful_pops;
	BasicPerformanceCounter<task_storage_count_successful_pops> num_successful_pops;
	BasicPerformanceCounter<task_storage_count_unsuccessful_takes> num_unsuccessful_takes;
	BasicPerformanceCounter<task_storage_count_successful_takes> num_successful_takes;
	BasicPerformanceCounter<task_storage_count_size_pop> total_size_pop;
	TimePerformanceCounter<task_storage_measure_pop_time> pop_time;
};

inline PrimitivePrimaryTaskStoragePerformanceCounters::PrimitivePrimaryTaskStoragePerformanceCounters() {

}

inline PrimitivePrimaryTaskStoragePerformanceCounters::PrimitivePrimaryTaskStoragePerformanceCounters(PrimitivePrimaryTaskStoragePerformanceCounters& other)
:num_unsuccessful_pops(other.num_unsuccessful_pops),
 num_successful_pops(other.num_successful_pops),
 num_unsuccessful_takes(other.num_unsuccessful_takes),
 num_successful_takes(other.num_successful_takes),
 total_size_pop(other.total_size_pop),
 pop_time(other.pop_time)
{

}

inline PrimitivePrimaryTaskStoragePerformanceCounters::~PrimitivePrimaryTaskStoragePerformanceCounters() {

}

inline void PrimitivePrimaryTaskStoragePerformanceCounters::print_headers() {
	BasicPerformanceCounter<task_storage_count_unsuccessful_pops>::print_header("num_unsuccessful_pops\t");
	BasicPerformanceCounter<task_storage_count_successful_pops>::print_header("num_successful_pops\t");
	BasicPerformanceCounter<task_storage_count_unsuccessful_takes>::print_header("num_unsuccessful_takes\t");
	BasicPerformanceCounter<task_storage_count_successful_takes>::print_header("num_successful_takes\t");
	BasicPerformanceCounter<task_storage_count_size_pop>::print_header("total_size_pop\t");
	TimePerformanceCounter<task_storage_measure_pop_time>::print_header("pop_time\t");
}

inline void PrimitivePrimaryTaskStoragePerformanceCounters::print_values() {
	num_unsuccessful_pops.print("%d\t");
	num_successful_pops.print("%d\t");
	num_unsuccessful_takes.print("%d\t");
	num_successful_takes.print("%d\t");
	total_size_pop.print("%d\t");
	pop_time.print("%f\t");
}

}

#endif /* PRIMITIVEPRIMARYTASKSTORAGEPERFORMANCECOUNTERS_H_ */
