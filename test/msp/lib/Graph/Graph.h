/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __GRAPH_H
#define __GRAPH_H

#include <stddef.h>
#include <vector>

namespace graph
{

typedef int weight_t;
typedef std::vector<weight_t> weight_vector_t;

struct Node;

struct Edge {
    Node* head;
    weight_vector_t weights;
};

struct Node {
    Edge* edges;
    size_t edge_count;
};

struct Graph {
    Node* nodes;
    size_t node_count;
    size_t degree;
};

Graph*
graph_random(const size_t vertices,
             const float edge_probability,
             const size_t degree,
             const unsigned int max_weight,
             const unsigned int seed);

void
graph_free(Graph* graph);

}

#endif /* __GRAPH_H */
