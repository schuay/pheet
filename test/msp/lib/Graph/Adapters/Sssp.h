/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef GRAPH_ADAPTERS_H_
#define GRAPH_ADAPTERS_H_

#include "lib/Graph/Graph.h"
#include "test/util/Graph/Graph.h"

namespace graph
{

class Sssp
{
public:
	graph::Graph* operator()(pheet::SsspGraph const* graph) const;
};

}

#endif /* GRAPH_ADAPTERS_H_ */
