/*
 * LocalityStrategyLUPiv.h
 *
 *  Created on: Jan 4, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LOCALITYSTRATEGYLUPIV_H_
#define LOCALITYSTRATEGYLUPIV_H_

#include "LocalityStrategyLUPivRootTask.h"

namespace pheet {

template <class Scheduler>
class LocalityStrategyLUPiv {
public:
	LocalityStrategyLUPiv(procs_t cpus, double* data, int* pivot, size_t size);
	~LocalityStrategyLUPiv();

	void run();
	void print_results();

	void print_headers();

	static void print_scheduler_name();

	static procs_t const max_cpus;
	static char const name[];
	static char const * const scheduler_name;

private:
	double* data;
	int* pivot;
	size_t size;
	typename Scheduler::CPUHierarchy cpu_hierarchy;
	Scheduler scheduler;
};

template <class Scheduler>
procs_t const LocalityStrategyLUPiv<Scheduler>::max_cpus = Scheduler::max_cpus;

template <class Scheduler>
char const LocalityStrategyLUPiv<Scheduler>::name[] = "LocalityStrategy LUPiv";

template <class Scheduler>
char const * const LocalityStrategyLUPiv<Scheduler>::scheduler_name = Scheduler::name;

template <class Scheduler>
LocalityStrategyLUPiv<Scheduler>::LocalityStrategyLUPiv(procs_t cpus, double* data, int* pivot, size_t size)
: data(data), pivot(pivot), size(size), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler>
LocalityStrategyLUPiv<Scheduler>::~LocalityStrategyLUPiv() {

}

template <class Scheduler>
void LocalityStrategyLUPiv<Scheduler>::run() {
	int n = (int)size;
	scheduler.template finish<LocalityStrategyLUPivRootTask<typename Scheduler::Task> >(data, pivot, n, n, n);
}

template <class Scheduler>
void LocalityStrategyLUPiv<Scheduler>::print_results() {
	scheduler.print_performance_counter_values();
}

template <class Scheduler>
void LocalityStrategyLUPiv<Scheduler>::print_headers() {
	scheduler.print_performance_counter_headers();
}

template <class Scheduler>
void LocalityStrategyLUPiv<Scheduler>::print_scheduler_name() {
	Scheduler::print_name();
}

}

#endif /* LOCALITYSTRATEGYLUPIV_H_ */
