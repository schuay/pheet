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

static SsspGraph*
graph_random(const size_t vertices,
             const float edge_probability,
             const unsigned int max_weight,
             const unsigned int seed)
{
	SsspGraphVertex* data = new SsspGraphVertex[vertices];

	std::mt19937 rng;
	rng.seed(seed);
	std::uniform_real_distribution<float> rnd_f(0.0, 1.0);
	std::uniform_int_distribution<size_t> rnd_st(1, max_weight);

	std::vector<SsspGraphEdge>* edges = new std::vector<SsspGraphEdge>[vertices];
	for (size_t i = 0; i < vertices; ++i) {
		for (size_t j = i + 1; j < vertices; ++j) {
			if (rnd_f(rng) < edge_probability) {
				SsspGraphEdge e;
				e.target = j;
				e.weight = rnd_st(rng);
				edges[i].push_back(e);
				e.target = i;
				edges[j].push_back(e);
			}
		}
		data[i].num_edges = edges[i].size();
		if (edges[i].size() > 0) {
			data[i].edges = new SsspGraphEdge[edges[i].size()];
			for (size_t j = 0; j < edges[i].size(); ++j) {
				data[i].edges[j] = edges[i][j];
			}
		} else {
			data[i].edges = NULL;
		}
		data[i].distance = std::numeric_limits<size_t>::max();
	}
	data[0].distance = 0;
	delete[] edges;

	return new SsspGraph { data, vertices };
}

static void
graph_free(SsspGraph* graph)
{
	for (size_t i = 0; i < graph->num_vertices; ++i) {
		if (graph->vertices[i].edges != NULL) {
			delete[] graph->vertices[i].edges;
		}
	}
	delete[] graph->vertices;
	delete graph;
}

}

#endif /* UTIL_GRAPH_H_ */
