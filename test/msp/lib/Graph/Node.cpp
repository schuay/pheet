/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Node.h"

namespace graph
{

uint
Node::
id() const
{
    return m_id;
}

Graph const*
Node::
graph() const
{
	return m_graph;
}

std::vector<Edge> const&
Node::
edges() const
{
    return m_edges;
}

Node::
Node(Graph const* graph,
     const uint id)
    : m_graph(graph), m_id(id)
{
}

}
