/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "MspBenchmarks.h"

#include "MspBenchmark.h"
#include "pheet/sched/BStrategy/BStrategyScheduler.h"
#include "pheet/sched/Synchroneous/SynchroneousScheduler.h"
#include "Sequential/SequentialMsp.h"
#include "Strategy/StrategyMspTask.h"

namespace
{
const size_t NODES         = 500;
const size_t EDGES         = 50000;
const unsigned int SEED    = 42;
const size_t WEIGHT_LIMIT  = 1000;
const size_t DEGREE        = 3;

template <class Pheet, template <class P> class Partitioner>
void
run_algorithm(graph::Graph* g, graph::Node* src, int n)
{
	pheet::MspBenchmark<Pheet, Partitioner> gbt(n, g, src, SEED);
	gbt.run_test();
}

}

namespace pheet
{

void
MspBenchmarks::
run_benchmarks(bool const sequential,
               bool const strategy,
               std::vector<int> const& n,
               std::vector<std::string> const& files)
{
	for (auto & it : files) {
		graph::Graph* g = graph::Graph::read(fopen(it.c_str(), "r"));
		graph::Node* src = g->nodes().front();

		/* Note: no need to execute with SynchroneousScheduler for different
		   amount of cores */
		if (sequential) {
			::run_algorithm < Pheet::WithScheduler<SynchroneousScheduler>,
			SequentialMsp > (g, src, 1);
		}

		for (auto & it : n) {
			if (strategy) {
				::run_algorithm < Pheet::WithScheduler<BStrategyScheduler>
				::WithTaskStorage<DistKStrategyTaskStorage>, StrategyMspTask > (g, src, it);
			}
		}

		delete g;
	}
}

} /* namespace pheet */
