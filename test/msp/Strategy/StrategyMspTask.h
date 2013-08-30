/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STRATEGYMSPTASK_H_
#define STRATEGYMSPTASK_H_

#include "../lib/Graph/Edge.h"
#include "../lib/Graph/Graph.h"
#include "../lib/Pareto/LockedSet.h"
#include "../lib/ShortestPath/ShortestPaths.h"
#include "../MspPerformanceCounters.h"
#include "StrategyMspStrategy.h"

namespace pheet
{

/**
 * Called by the scheduler for each selected path taken from the Pareto queue.
 *
 * The rough steps performed by a task are:
 * 1. Generate candidates.
 * 2. For each candidate c, insert it into the global Pareto set.
 *    If c is dominated, return.
 *    Otherwise, spawn a new task for c and mark all tasks as dead
 *    which have been pruned from the Pareto set by the insertion.
 *
 * Since there is no strictly defined sequence, this path is not necessarily
 * a Pareto optimum <-> this is a label correcting algorithm.
 *
 * We deviate from the Sanders algorithm in that parallelism
 * is only achieved by path; once a path has been selected and its corresponding
 * task has been spawned, that task executes sequentially (but in parallel
 * with other similar tasks) until finally spawning new potential tasks before
 * finishing.
 *
 * Furthermore, we lose some of the parallelism achieved by Sanders by
 * grouping candidates by node and doing an initial prune within each node set
 * in parallel before attempting to update the Pareto set. This causes a larger
 * amount of accesses for theglobal Pareto set. We are also unable to perform
 * bulk updates, since individual tasks do not communicate with each other.
 */

template <class Pheet>
class StrategyMspTask : public Pheet::Task
{
public:
	typedef StrategyMspTask<Pheet> Self;
	typedef StrategyMspStrategy<Pheet> Strategy;
	typedef MspPerformanceCounters<Pheet> PerformanceCounters;

	StrategyMspTask(const graph::Graph* graph,
	                const sp::PathPtr path,
	                pareto::Set* set,
	                PerformanceCounters& pc);

	virtual void operator()();

	static void set_k(size_t k);

	static char const name[];

private:
	const graph::Graph* graph;
	const sp::PathPtr path;

	pareto::Set* set;

	PerformanceCounters& pc;
};

template <class Pheet>
char const StrategyMspTask<Pheet>::name[] = "Strategy Msp";

template <class Pheet>
StrategyMspTask<Pheet>::
StrategyMspTask(const graph::Graph* graph,
                const sp::PathPtr path,
                pareto::Set* set,
                PerformanceCounters& pc)
	: graph(graph), path(path), set(set), pc(pc)
{
}

template <class Pheet>
void
StrategyMspTask<Pheet>::
operator()()
{
	pc.num_actual_tasks.incr();

	/* Generate candidates. */
	const graph::Node* head = path->head();

	sp::Paths candidates;
	for (auto & e : head->out_edges()) {
		sp::PathPtr to(path->step(e));
		candidates.push_back(to);
	}

	sp::Paths non_dominated = set->insert(candidates);
	for (sp::PathPtr & p : non_dominated) {
		Pheet::template spawn_s<Self>(Strategy(p->weight()), graph, p, set, pc);
	}
}

template <class Pheet>
void
StrategyMspTask<Pheet>::
set_k(size_t k)
{
	Strategy::default_k = k;
}

} /* namespace pheet */

#endif /* STRATEGYMSPTASK_H_ */
