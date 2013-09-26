/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STRATEGYMSPTASK_H_
#define STRATEGYMSPTASK_H_

#include "MspPerformanceCounters.h"
#include "StrategyMspStrategy.h"
#include "lib/Graph/Edge.h"
#include "lib/Graph/Graph.h"
#include "lib/Pareto/Sets.h"
#include "lib/ShortestPath/ShortestPaths.h"

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
 * amount of accesses for the global Pareto set. We are also unable to perform
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
	                pareto::Sets* sets,
	                PerformanceCounters& pc);

	virtual void operator()();

	static void set_k(size_t k);

	static char const name[];

private:
	const graph::Graph* graph;
	const sp::PathPtr path;

	pareto::Sets* sets;

	PerformanceCounters& pc;
};

template <class Pheet>
char const StrategyMspTask<Pheet>::name[] = "Strategy Msp";

template <class Pheet>
StrategyMspTask<Pheet>::
StrategyMspTask(const graph::Graph* graph,
                const sp::PathPtr path,
                pareto::Sets* sets,
                PerformanceCounters& pc)
	: graph(graph), path(path), sets(sets), pc(pc)
{
}

template <class Pheet>
void
StrategyMspTask<Pheet>::
operator()()
{
	if (path->dominated()) {
		return;
	}

	pc.num_actual_tasks.incr();

	/* Generate candidates. */
	const graph::Node* head = path->head();

	sp::Paths candidates;
	candidates.reserve(head->out_edges().size());
	for (auto & e : head->out_edges()) {
		sp::PathPtr to(path->step(e));
		candidates.push_back(to);
	}

	/* Insert into the Pareto set. Mark dominated paths and spawn tasks for
	 * newly added paths. */

	sp::Paths added, removed;
	sets->insert(candidates, added, removed);

	for (sp::PathPtr & p : removed) {
		p->set_dominated();
		pc.num_dead_tasks.incr();
	}

	for (sp::PathPtr & p : added) {
		Pheet::template spawn_s<Self>(Strategy(p), graph, p, sets, pc);
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
