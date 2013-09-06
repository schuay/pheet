/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Graph.h"

#include <random>

namespace graph
{

Graph*
graph_random(const size_t node_count,
             const float edge_probability,
             const size_t degree,
             const unsigned int max_weight,
             const unsigned int seed)
{
    Node* nodes = new Node[node_count];

    std::mt19937 rng;
    rng.seed(seed);

    std::uniform_real_distribution<float> rnd_f(0.0, 1.0);
    std::uniform_int_distribution<size_t> rnd_st(1, max_weight);

    std::vector<Edge>* edges = new std::vector<Edge>[node_count];
    for (size_t i = 0; i < node_count; ++i) {
        for (size_t j = i + 1; j < node_count; ++j) {
            if (rnd_f(rng) < edge_probability) {
                weight_vector_t ws;
                for (size_t k = 0; k < degree; k++) {
                    ws.push_back(rnd_st(rng));
                }

                edges[i].push_back({ nodes + j, ws });
                edges[j].push_back({ nodes + i, ws });
            }
        }
        nodes[i].edge_count = edges[i].size();
        nodes[i].edges = new Edge[edges[i].size()];
        for (size_t j = 0; j < edges[i].size(); ++j) {
            nodes[i].edges[j] = edges[i][j];
        }
    }
    delete[] edges;

    return new Graph { nodes, node_count, degree };
}

void
graph_free(Graph* graph)
{
    for (size_t i = 0; i < graph->node_count; ++i) {
        delete[] graph->nodes[i].edges;
    }
    delete[] graph->nodes;
    delete graph;
}

}
