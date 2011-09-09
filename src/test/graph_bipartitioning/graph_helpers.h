/*
 * graph_helpers.h
 *
 *  Created on: 07.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef GRAPH_HELPERS_H_
#define GRAPH_HELPERS_H_

#include "../../misc/types.h"
#include "../../primitives/Reducer/Max/MaxReducer.h"

#include <set>

namespace pheet {

struct GraphEdge {
	size_t target;
	size_t weight;
};

struct GraphVertex {
	GraphEdge* edges;
	size_t num_edges;
};

struct GraphBipartitioningSolution {
	size_t weight;
	std::set<size_t> set1;
	std::set<size_t> set2;
};

template <>
struct MaxOperation<GraphBipartitioningSolution> {
	GraphBipartitioningSolution& operator()(GraphBipartitioningSolution& x, GraphBipartitioningSolution& y);

	GraphBipartitioningSolution get_identity();
};

}

#endif /* GRAPH_HELPERS_H_ */
