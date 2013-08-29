/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef MSPTEST_H_
#define MSPTEST_H_

#include "../Test.h"
#include "pheet/pheet.h"

#include "lib/Generator/Generator.h"

namespace pheet
{

template <class Pheet, template <class P> class Algorithm>
class MspTest : Test
{
public:
	/**
	 * MspTest executes a single run of the multi-criteria shortest path algorithm
	 * with the given parameters.
	 */
	MspTest(const procs_t cpus,
	        const size_t nodes,
	        const size_t edges,
	        const graph::Generator::Wl& weight_limits,
	        const unsigned int seed);
	virtual ~MspTest();

	void run_test();

private:
	procs_t cpus;
	graph::Graph* g;
	unsigned int seed;
	graph::Node* src;

	static char const* const types[];
};

template <class Pheet, template <class P> class Algorithm>
char const* const MspTest<Pheet, Algorithm>::types[] = {"random"};

template <class Pheet, template <class P> class Algorithm>
MspTest<Pheet, Algorithm>::MspTest(const procs_t cpus,
                                   const size_t nodes,
                                   const size_t edges,
                                   const graph::Generator::Wl& weight_limits,
                                   const unsigned int seed)
	: cpus(cpus), seed(seed)
{
	g = graph::Generator::directed("test", nodes, edges, true, weight_limits, seed);
	src = g->nodes().front();
}

template <class Pheet, template <class P> class Algorithm>
MspTest<Pheet, Algorithm>::~MspTest()
{
	delete g;
}

template <class Pheet, template <class P> class Algorithm>
void MspTest<Pheet, Algorithm>::run_test()
{
	typename Pheet::Environment::PerformanceCounters pc;
	typename Algorithm<Pheet>::PerformanceCounters ppc;

	Time start, end;

	{
		typename Pheet::Environment env(cpus, pc);

		check_time(start);
		Pheet::template finish<Algorithm<Pheet>>(g, src, ppc);
		check_time(end);
	}

	const double seconds = calculate_seconds(start, end);

	/*TODO: doesn't print anything for SynchronousScheduler
	    see: /pheet/sched/Synchroneous/SynchroneousSchedulerPerformanceCounters.h
	*/
	//Pheet::Environment::PerformanceCounters::print_headers();
	std::cout << std::endl;
	std::cout << "msp";
	std::cout << "\t" << Algorithm<Pheet>::name;
	std::cout << "\t";
	Pheet::Environment::print_name();
	std::cout << "\ttpye";
	std::cout << "\t" << g->node_count();
	std::cout << "\t" << g->edge_count();
	std::cout << "\t" << seed;
	std::cout << "\t" << cpus;
	std::cout << "\t" << seconds;
	std::cout << "\t";
	//pc.print_values();
	std::cout << std::endl;
}

} /* namespace pheet */

#endif /* MSPTEST_H_ */
