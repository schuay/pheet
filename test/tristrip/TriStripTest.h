/*
* TriStripTests.h
*
*  Created on: 5.12.2011
*      Author: Daniel Cederman
*     License: Boost Software License 1.0 (BSL1.0)
*/

#ifndef TriStripTEST_H_
#define TriStripTEST_H_

#include "../../pheet/settings.h"
#include "../../pheet/misc/types.h"
#include "../Test.h"
#include "GraphDual.h"

#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

namespace pheet {

template <class Impl>
class TriStripTest : Test {
public:
    TriStripTest(procs_t cpus, size_t nodecount, bool withstrat);
	~TriStripTest();

	void run_test();

private:
	procs_t cpus;
	size_t nodecount;
	bool withstrat;
};

template <class Impl>
  TriStripTest<Impl>::TriStripTest(procs_t cpus, size_t nodecount, bool withstrat)
  : cpus(cpus),nodecount(nodecount),withstrat(withstrat){

}

template <class Impl>
TriStripTest<Impl>::~TriStripTest() {

}

template <class Impl>
void TriStripTest<Impl>::run_test() {

	GraphDual graph(nodecount,123,0.4);

	printf("Start\n");

	Impl iar(cpus, graph, withstrat);

	Time start, end;
	check_time(start);
	iar.run();
	check_time(end);

	double seconds = calculate_seconds(start, end);
	std::cout << "test\timplementation\tscheduler\tcpus\ttotal_time\ttristrips\tnodesintristrips\t";
	iar.print_headers();
	std::cout << std::endl;
	std::cout << "tristrip\t";
	Impl::print_name();
	std::cout << "\t";
	Impl::print_scheduler_name();
	std::cout << "\t" << cpus << "\t" << seconds << "\t";
	std::cout << /*nodecount << "\t" <<*/ iar.getTriStripCount() << "\t" << iar.getNodeTriStripCount() << "\t";
	iar.print_results();
	std::cout << std::endl;
}
}

#endif /* TriStripTESTS_H_ */
