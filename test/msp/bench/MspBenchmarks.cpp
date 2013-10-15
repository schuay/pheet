/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "MspBenchmarks.h"

#include "MspBenchmark.h"
#include "pheet/sched/BStrategy/BStrategyScheduler.h"
#include "pheet/sched/Strategy2/StrategyScheduler2.h"
#include "pheet/sched/Synchroneous/SynchroneousScheduler.h"
#include "Sequential/SequentialMsp.h"
#include "Strategy/StrategyMspTask.h"
#include "Strategy2/Strategy2MspTask.h"

namespace
{

template <class Pheet, template <class P> class Partitioner>
void
run_algorithm(graph::Graph* g, graph::Node* src, int n)
{
	pheet::MspBenchmark<Pheet, Partitioner> gbt(n, g, src);
	gbt.run_test();
}

}

namespace pheet
{

void
MspBenchmarks::
run_benchmarks(BenchOpts const& opts)
{
	for (auto & it : opts.files) {
		graph::Graph* g = graph::Graph::read(fopen(it.c_str(), "r"));
		graph::Node* src = g->nodes().front();

		/* Note: no need to execute with SynchroneousScheduler for different
		   amount of cores */
		if (opts.sequential) {
			for (int i = 0; i < opts.repetitions; i++) {
				::run_algorithm < Pheet::WithScheduler<SynchroneousScheduler>,
				SequentialMsp > (g, src, 1);
			}
		}

		if (opts.strategy) {
			for (auto & it : opts.ncpus) {
				for (int i = 0; i < opts.repetitions; i++) {
					::run_algorithm < Pheet::WithScheduler<BStrategyScheduler>
					::WithTaskStorage<DistKStrategyTaskStorage>, StrategyMspTask > (g, src, it);
				}
			}
		}

		if (opts.strategy2) {
			for (auto & it : opts.ncpus) {
				for (int i = 0; i < opts.repetitions; i++) {
					::run_algorithm < Pheet::WithScheduler<StrategyScheduler2>, Strategy2MspTask > (g, src, it);

				}
			}
		}

		delete g;
	}


}

} /* namespace pheet */
