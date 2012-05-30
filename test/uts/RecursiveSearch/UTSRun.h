/*
* UTSRun.h
*
*  Created on: 22.09.2011
*      Author: Daniel Cederman
*     License: Boost Software License 1.0 (BSL1.0)
*/

#ifndef UTSRUN_H_
#define UTSRUN_H_

#include <pheet/pheet.h>
#include "UTSTasks.h"

#include <iostream>

namespace pheet {

template <class Pheet>
class UTSRun {
public:
	UTSRun(procs_t cpus);
	~UTSRun();

	void run();

	void print_results();
	void print_headers();

	static void print_name();
	static void print_scheduler_name();

//	static procs_t const max_cpus;
//	static char const name[];
private:
    procs_t cpus;
//	typename Scheduler::CPUHierarchy cpu_hierarchy;
//	Scheduler scheduler;
};

//template <class Scheduler>
//procs_t const UTSRun<Scheduler>::max_cpus = Scheduler::max_cpus;

//template <class Scheduler>
//char const UTSRun<Scheduler>::name[] = "RecursiveSearch";

template <class Pheet>
UTSRun<Pheet>::UTSRun(procs_t cpus): cpus(cpus) {

}

template <class Pheet>
UTSRun<Pheet>::~UTSRun() {

}

template <class Pheet>
void UTSRun<Pheet>::run() {
	
	Node root;
	uts_initRoot(&root, type);
	typename Pheet::Environment env(cpus);
	Pheet::template finish<UTSStartTask<Pheet> >(root);
}

template <class Pheet>
void UTSRun<Pheet>::print_results() {
//	scheduler.print_performance_counter_values();
}

template <class Pheet>
void UTSRun<Pheet>::print_headers() {
//	scheduler.print_performance_counter_headers();
}

template <class Pheet>
void UTSRun<Pheet>::print_name() {
//	std::cout << name;
}

template <class Pheet>
void UTSRun<Pheet>::print_scheduler_name() {
//	Scheduler::print_name();
}
}

#endif /* UTSRUN_H_ */
