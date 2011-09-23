/*
 * DagQuicksort.h
 *
 *  Created on: 08.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef DAGQUICKSORT_H_
#define DAGQUICKSORT_H_

#include "../../../misc/types.h"
#include "../../../misc/atomics.h"
#include "DagQuicksortTask.h"

namespace pheet {

template <class Scheduler>
class DagQuicksort {
public:
	DagQuicksort(procs_t cpus, unsigned int* data, size_t length);
	~DagQuicksort();

	void sort();
	void print_results();

	void print_headers();

	static void print_scheduler_name();

	static procs_t const max_cpus;
	static char const name[];
	static char const * const scheduler_name;

private:
	unsigned int* data;
	size_t length;
	typename Scheduler::CPUHierarchy cpu_hierarchy;
	Scheduler scheduler;
};

template <class Scheduler>
procs_t const DagQuicksort<Scheduler>::max_cpus = Scheduler::max_cpus;

template <class Scheduler>
char const DagQuicksort<Scheduler>::name[] = "Dag Quicksort";

template <class Scheduler>
char const * const DagQuicksort<Scheduler>::scheduler_name = Scheduler::name;

template <class Scheduler>
DagQuicksort<Scheduler>::DagQuicksort(procs_t cpus, unsigned int *data, size_t length)
: data(data), length(length), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler>
DagQuicksort<Scheduler>::~DagQuicksort() {

}

template <class Scheduler>
void DagQuicksort<Scheduler>::sort() {
	scheduler.template finish<DagQuicksortTask<typename Scheduler::Task> >(data, length);
}

template <class Scheduler>
void DagQuicksort<Scheduler>::print_results() {
	scheduler.print_performance_counter_values();
}

template <class Scheduler>
void DagQuicksort<Scheduler>::print_headers() {
	scheduler.print_performance_counter_headers();
}

template <class Scheduler>
void DagQuicksort<Scheduler>::print_scheduler_name() {
	Scheduler::print_name();
}

}

#endif /* DAGQUICKSORT_H_ */
