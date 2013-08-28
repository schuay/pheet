/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "MspTests.h"

#include "MspTest.h"
#include "pheet/sched/Synchroneous/SynchroneousScheduler.h"
#include "Sequential/SequentialMsp.h"
#include "Strategy/StrategyMspStrategy.h"
#include "Strategy/StrategyMspTask.h"

namespace
{
const size_t NODES      = 50;
const size_t EDGES      = 500;
const unsigned int SEED = 42;

template <class Pheet, template <class P> class Partitioner>
void run_algorithm()
{
	typename Pheet::MachineModel mm;
	const pheet::procs_t max_cpus =
	    std::min(mm.get_num_leaves(), Pheet::Environment::max_cpus);

	pheet::MspTest<Pheet, Partitioner> gbt(max_cpus, NODES, EDGES,
	                                       graph::Generator::default_weights(), SEED);
	gbt.run_test();
}

}

namespace pheet
{

void MspTests::run_test()
{
#ifdef MSP_TEST
	/* Print header information. */
	std::cout << "test\t"
	          << "algorithm\t"
	          << "scheduler\t"
	          << "type\t"
	          << "nodes\t"
	          << "edges\t"
	          << "seed\t"
	          << "cpus\t"
	          << "total_time\t"
	          << "correct";

	::run_algorithm < Pheet::WithScheduler<SynchroneousScheduler>,
	SequentialMsp > ();
#endif
}

} /* namespace pheet */
