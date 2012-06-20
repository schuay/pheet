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

	static char const name[];
private:
    procs_t cpus;
};

template <class Pheet>
char const UTSRun<Pheet>::name[] = "UTSRecursiveSearch";

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
	Pheet::print_performance_counter_values();
}

template <class Pheet>
void UTSRun<Pheet>::print_headers() {
	Pheet::print_performance_counter_headers();
}

template <class Pheet>
void UTSRun<Pheet>::print_name() {
	std::cout << name;
}

template <class Pheet>
void UTSRun<Pheet>::print_scheduler_name() {
	Pheet::print_name();
}
}

#endif /* UTSRUN_H_ */
