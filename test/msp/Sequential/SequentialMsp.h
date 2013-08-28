#ifndef __SEQUENTIAL_MSP_H
#define __SEQUENTIAL_MSP_H

#include "../lib/Graph/Edge.h"
#include "../lib/Graph/Graph.h"
#include "../lib/Pareto/Less.h"
#include "../lib/Pareto/PriorityLinkedQueue.h"
#include "../lib/ShortestPath/ShortestPaths.h"

namespace pheet
{

class SequentialMsp
{
public:
	SequentialMsp(const graph::Graph* graph,
	              const graph::Node* start);

	sp::ShortestPaths*
	operator()();

private:
	const graph::Graph* graph;
	const graph::Node* start;

	pareto::less dominates;
	pareto::PriorityLinkedQueue m_queue;
};

}

#endif /* __SEQUENTIAL_MSP_H */
