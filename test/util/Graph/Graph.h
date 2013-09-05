/*
 *  Created on: Jul 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef UTIL_GRAPH_H_
#define UTIL_GRAPH_H_

#include <random>

namespace pheet
{

struct SsspGraphEdge {
	size_t target;
	size_t weight;
};

struct SsspGraphVertex {
	SsspGraphEdge* edges;
	size_t num_edges;
	size_t distance;
};

struct SsspGraph {
	SsspGraphVertex* vertices;
	size_t num_vertices;
};

SsspGraph*
graph_random(const size_t vertices,
             const float edge_probability,
             const unsigned int max_weight,
             const unsigned int seed);

void
graph_free(SsspGraph* graph);

}

#endif /* UTIL_GRAPH_H_ */
