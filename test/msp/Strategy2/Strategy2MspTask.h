/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STRATEGY2MSPTASK_H_
#define STRATEGY2MSPTASK_H_

#include "lib/Pareto/Sets.h"
#include "MspPerformanceCounters.h"
#include "pheet/ds/StrategyTaskStorage/ParetoLocality/ParetoLocalityTaskStorage.h"
#include "Strategy2MspStrategy.h"

namespace pheet
{

template <class Pheet, template <class, class> class TaskStorageT>
class Strategy2MspTask : public Pheet::Task
{
public:
	typedef Strategy2MspTask<Pheet, TaskStorageT> Self;
	typedef Strategy2MspStrategy<Pheet, TaskStorageT> Strategy;
	typedef MspPerformanceCounters<Pheet> PerformanceCounters;


	Strategy2MspTask(const graph::Graph* graph,
	                 const sp::PathPtr path,
	                 pareto::Sets* sets,
	                 PerformanceCounters& pc);

	virtual void operator()();

	static char const name[];


private:
	const graph::Graph* graph;
	const sp::PathPtr path;
	pareto::Sets* sets;
	PerformanceCounters& pc;
};

template <class Pheet, template <class, class> class TaskStorageT>
char const Strategy2MspTask<Pheet, TaskStorageT>::
name[] = "Strategy2 Msp";

template <class Pheet, template <class, class> class TaskStorageT>
Strategy2MspTask<Pheet, TaskStorageT>::
Strategy2MspTask(const graph::Graph* graph,
                 const sp::PathPtr path,
                 pareto::Sets* sets,
                 PerformanceCounters& pc)
	: graph(graph), path(path), sets(sets), pc(pc)
{
}

template <class Pheet, template <class, class> class TaskStorageT>
void
Strategy2MspTask<Pheet, TaskStorageT>::
operator()()
{
	if (path->dominated()) {
		return;
	}

	pc.num_actual_tasks.incr();

	StrategyMspData& d = Pheet::template place_singleton<StrategyMspData>();
	d.clear();

	/* Generate candidates. */
	const graph::Node* head = path->head();

	d.candidates.reserve(head->out_edges().size());
	for (auto & e : head->out_edges()) {
		sp::PathPtr to(path->step(e));
		d.candidates.push_back(to);
	}

	/* Insert into the Pareto set. Mark dominated paths and spawn tasks for
	 * newly added paths. */
	sets->insert(d.candidates, d.added, d.removed);

	for (sp::PathPtr & p : d.removed) {
		p->set_dominated();
		pc.num_dead_tasks.incr();
	}

	for (sp::PathPtr & p : d.added) {
		Pheet::template spawn_s<Self>(Strategy(p), graph, p, sets, pc);
	}
}

template <class Pheet>
using Strategy2Msp = Strategy2MspTask<Pheet, ParetoLocalityTaskStorage>;

} /* namespace pheet */

#endif /* STRATEGY2MSPTASK_H_ */

