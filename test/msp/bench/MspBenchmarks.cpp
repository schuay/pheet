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
run_algorithm(graph::Graph* g, graph::Node* src)
{
	typename Pheet::MachineModel mm;
	const pheet::procs_t max_cpus =
	    std::min(mm.get_num_leaves(), Pheet::Environment::max_cpus);

	pheet::MspBenchmark<Pheet, Partitioner> gbt(max_cpus, g, src, SEED);
	gbt.run_test();
}

}

namespace pheet
{

void
MspBenchmarks::
run_benchmarks()
{
	graph::Graph* g = graph::Generator::directed("test", NODES, EDGES, true,
	                  DEGREE, WEIGHT_LIMIT, SEED);
	graph::Node* src = g->nodes().front();

	::run_algorithm<Pheet::WithScheduler<SynchroneousScheduler>, SequentialMsp>(g, src);

	::run_algorithm < Pheet::WithScheduler<BStrategyScheduler>
	::WithTaskStorage<DistKStrategyTaskStorage>, StrategyMspTask > (g, src);
}

} /* namespace pheet */
