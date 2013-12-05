/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __SEQUENTIAL_MSP_H
#define __SEQUENTIAL_MSP_H

#include "MspPerformanceCounters.h"
#include "lib/Graph/Edge.h"
#include "lib/Graph/Graph.h"
#include "lib/Pareto/Less.h"
#include "lib/Pareto/PriorityQueue.h"
#include "lib/Pareto/Sets.h"

namespace pheet
{

template <class Pheet>
class SequentialMsp
{
public:
	typedef MspPerformanceCounters<Pheet> PerformanceCounters;

	SequentialMsp(graph::Graph const* graph,
	              sp::PathPtr const path,
	              pareto::Sets* sets,
	              PerformanceCounters& pc);

	void
	operator()();

	static void print_name();
private:
	static char const name[];

private:
	graph::Graph const* graph;
	sp::PathPtr const path;

	pareto::PriorityQueue m_queue;
	pareto::Sets* sets;

	PerformanceCounters& pc;
};
}

#endif /* __SEQUENTIAL_MSP_H */
