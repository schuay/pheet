/*
 * SimpleLUPiv.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef SIMPLELUPIV_H_
#define SIMPLELUPIV_H_

#include "SimpleLUPivRootTask.h"

namespace pheet {

template <class Scheduler>
class SimpleLUPiv {
public:
	SimpleLUPiv(procs_t cpus, double* data, int* pivot, size_t size);
	~SimpleLUPiv();

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
procs_t const SimpleLUPiv<Scheduler>::max_cpus = Scheduler::max_cpus;

template <class Scheduler>
char const SimpleLUPiv<Scheduler>::name[] = "Simple LUPiv";

template <class Scheduler>
char const * const SimpleLUPiv<Scheduler>::scheduler_name = Scheduler::name;

template <class Scheduler>
SimpleLUPiv<Scheduler>::SimpleLUPiv(procs_t cpus, double* data, int* pivot, size_t size)
: data(data), pivot(pivot), size(size), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler>
SimpleLUPiv<Scheduler>::~SimpleLUPiv() {

}

template <class Scheduler>
void SimpleLUPiv<Scheduler>::run() {
	int n = (int)size;
	scheduler.template finish<SimpleLUPivRootTask<typename Scheduler::Task> >(data, pivot, n, n, n);
}

template <class Scheduler>
void SimpleLUPiv<Scheduler>::print_results() {
	scheduler.print_performance_counter_values();
}

template <class Scheduler>
void SimpleLUPiv<Scheduler>::print_headers() {
	scheduler.print_performance_counter_headers();
}

template <class Scheduler>
void SimpleLUPiv<Scheduler>::print_scheduler_name() {
	Scheduler::print_name();
}

}

#endif /* SIMPLELUPIV_H_ */
