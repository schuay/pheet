/*
* InARow.h
*
*  Created on: 22.09.2011
*      Author: Daniel Cederman
*     License: Boost Software License 1.0 (BSL1.0)
*/

#ifndef TriStripTASK_H_
#define TriStripTASK_H_

#include <pheet/pheet.h>
#include "../../../pheet/misc/types.h"
#include "../../../pheet/misc/atomics.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <exception>
#include <queue>

#include "../GraphDual.h"
#include "TriStripPerformanceCounters.h"
#include "LowDegreeStrategy.h"
#include "TriStripResult.h"

using namespace std;

namespace pheet {

	template <class Pheet>
	class TriStripSliceTask;

	template <class Pheet>
	class TriStripStartTask : public Pheet::Task
	{
		GraphDual& graph;
		TriStripResult<Pheet> result;
		TriStripPerformanceCounters<Pheet> pc;

	public:

		TriStripStartTask(GraphDual& graph, TriStripResult<Pheet>& result, TriStripPerformanceCounters<Pheet>& pc):graph(graph),result(result),pc(pc) { }
		virtual ~TriStripStartTask() {}

		void operator()()
		{

			size_t spawnsetcount = 128;

			for(size_t i=0; i<graph.size(); i+=spawnsetcount)
			{
				typename Pheet::Finish f;
				for(size_t r = 0; r < spawnsetcount; r++)
				{
					if(!graph[i+r]->taken && !graph[i+r]->spawned_hint)
					{
						graph[i+r]->spawned_hint = true;
					    Pheet::template spawn_s<TriStripSliceTask<Pheet>>(LowDegreeStrategy<Pheet>(graph,graph[i+r]),graph,graph[i+r],result,pc);

					}
				}
			}

		}
	};

	class NodeWithDegree
	{
		GraphNode* n;
		size_t degree;

	public:

		NodeWithDegree(GraphNode* n, size_t degree):n(n),degree(degree)
		{


		}

		GraphNode* getNode() const
		{
			return n;
		}

		bool operator < (const NodeWithDegree& n) const
		{
			return degree > n.degree;
		}

	};

	template <class Pheet>
	class TriStripSliceTask : public Pheet::Task
	{

		GraphDual& graph;
		GraphNode* startnode;
		TriStripResult<Pheet> result;
		TriStripPerformanceCounters<Pheet> pc;
	public:

		TriStripSliceTask(GraphDual& graph, GraphNode* startnode, TriStripResult<Pheet>& result, TriStripPerformanceCounters<Pheet>& pc):graph(graph), startnode(startnode),result(result),pc(pc) {}
		virtual ~TriStripSliceTask() {}

		void operator()()
		{
			std::vector<GraphNode*> strip;

			std::priority_queue<NodeWithDegree> possiblenextnodes;

			GraphNode* currnode = startnode;
			if(!currnode->take())
				return;

			strip.push_back(currnode);

			while(true)
			{

				for(int d=0;d<currnode->num_edges;d++)
				{
					GraphNode* possnode = currnode->edges[d];

					if(!possnode->isTaken())
					{
						possiblenextnodes.push(NodeWithDegree(possnode,possnode->getDegree()));
					}
				}

				// TODO SGI has a check on next level of degree of the node

				bool found = false;

				while(!possiblenextnodes.empty())
				{
					GraphNode* g = (possiblenextnodes.top().getNode());
					if(g->take())
					{
						found = true;
						currnode = g;
						strip.push_back(g);
						break;
					}
					possiblenextnodes.pop();
				}

				if(!found)
					break;

				while(!possiblenextnodes.empty())
				{
					GraphNode* n = possiblenextnodes.top().getNode();

					if(!n->spawned_hint)
					{
						n->spawned_hint = true;
						Pheet::template spawn_s<TriStripSliceTask<Pheet>>(LowDegreeStrategy<Pheet>(graph,n),graph,n,result,pc);
					}

					possiblenextnodes.pop();
				}

			}

			result.addstrip(strip);


		}

	};
}
#endif
