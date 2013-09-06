/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "SequentialMsp.h"

#include <memory>

#include "pheet/pheet.h"
#include "pheet/sched/Synchroneous/SynchroneousScheduler.h"

using namespace graph;
using namespace sp;

namespace pheet
{

template <class Pheet>
SequentialMsp<Pheet>::
SequentialMsp(Graph const* graph,
              sp::PathPtr const path,
              pareto::Sets* sets,
              PerformanceCounters& pc)
	: graph(graph), path(path), sets(sets), pc(pc)
{
}

template <class Pheet>
void
SequentialMsp<Pheet>::
operator()()
{
	PathPtr init = path;
	m_queue.insert(init);

	while (!m_queue.empty()) {
		/* Retrieve our next optimal candidate path. */

		PathPtr p = m_queue.first();
		Node const* head = p->head();

		pc.num_actual_tasks.incr();

		/* For all outgoing edges <- head, generate candidates. */

		sp::Paths candidates;
        for (size_t i = 0; i < head->edge_count; i++) {
            Edge *e = head->edges + i;
			sp::PathPtr q(p->step(e));
			candidates.push_back(q);
		}

		sp::Paths added, removed;
		sets->insert(candidates, added, removed);

		/* Mark removed candidates as dominated. */

		for (auto & p : removed) {
			p->set_dominated();
		}

		/* Add newly inserted candidates to our queue. */

		for (auto & p : added) {
			m_queue.insert(p);
		}
	}
}

template class SequentialMsp < pheet::PheetEnv < pheet::SynchroneousScheduler,
         pheet::SystemModel,
         pheet::Primitives,
         pheet::DataStructures,
         pheet::ConcurrentDataStructures >>;

}
