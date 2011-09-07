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

namespace pheet {

struct GraphEdge {
	size_t target;
	size_t weight;
};

struct GraphVertex {
	GraphEdge* edges;
	size_t num_edges;
};

}

#endif /* GRAPH_HELPERS_H_ */
