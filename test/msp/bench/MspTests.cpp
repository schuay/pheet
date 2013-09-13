/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "MspTests.h"

#include "MspTest.h"
#include "pheet/sched/BStrategy/BStrategyScheduler.h"
#include "pheet/sched/Synchroneous/SynchroneousScheduler.h"
#include "Sequential/SequentialMsp.h"
#include "Strategy/StrategyMspTask.h"

namespace
{
const size_t NODES      = 500;
const size_t EDGES      = 50000;
const unsigned int SEED = 42;

template <class Pheet, template <class P> class Partitioner>
void run_algorithm(graph::Graph* g, graph::Node* src)
{
	typename Pheet::MachineModel mm;
	const pheet::procs_t max_cpus =
	    std::min(mm.get_num_leaves(), Pheet::Environment::max_cpus);

	pheet::MspTest<Pheet, Partitioner> gbt(1, g, src, SEED);
	gbt.run_test();
}

}

namespace pheet
{

void MspTests::run_test()
{
	graph::Graph* g = graph::Generator::directed("test", NODES, EDGES, true,
	                  graph::Generator::default_weights(), SEED);
	graph::Node* src = g->nodes().front();

	::run_algorithm<Pheet::WithScheduler<SynchroneousScheduler>, SequentialMsp>(g, src);

	::run_algorithm < Pheet::WithScheduler<BStrategyScheduler>
	::WithTaskStorage<DistKStrategyTaskStorage>, StrategyMspTask > (g, src);
}

} /* namespace pheet */
