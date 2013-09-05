/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Sssp.h"

#include <vector>

using namespace pheet;

namespace graph
{

Graph*
Sssp::
operator()(SsspGraph const* graph) const
{
	Graph* g = new Graph("Sssp", 1);

	std::vector<Node*> vertices;
	for (size_t i = 0; i < graph->num_vertices; i++) {
		vertices.push_back(g->add_node());
	}

	for (size_t i = 0; i < graph->num_vertices; i++) {
		SsspGraphVertex const* vertex = graph->vertices + i;

		for (size_t j = 0; j < vertex->num_edges; j++) {
			SsspGraphEdge const* edge = vertex->edges + j;
			g->add_edge(vertices[i], vertices[edge->target], { (int)edge->weight });
		}
	}

	return g;
}

}
