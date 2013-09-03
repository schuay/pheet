/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __NODE_H
#define __NODE_H

#include <vector>

#include "Graph.h"

namespace graph
{

class Node
{
	friend class Edge;
	friend class Graph;

public:
	ulong id() const;

	Graph* graph() const;

	std::vector<Edge const*> out_edges() const;

private:
	Node(Graph* graph);
	Node(Graph* graph, Agnode_t* n);

private:
	Graph* m_graph;
	Agnode_t* n;

	ulong m_id;

	std::vector<Edge const*> m_out_edges;
};

}

#endif /* __NODE_H */
