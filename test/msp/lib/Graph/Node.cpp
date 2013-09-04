/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Node.h"

#include <graphviz/cgraph.h>

namespace graph
{

ulong
Node::
id() const
{
	return AGID(n);
}

Graph*
Node::
graph() const
{
	return m_graph;
}

std::vector<Edge const*> const&
Node::
out_edges() const
{
	return m_out_edges;
}

Node::
Node(Graph* graph)
	: m_graph(graph)
{
	n = agnode(m_graph->g, NULL, TRUE);
	m_id = AGID(n);
	agset(n, ATTR_NODEID, const_cast<char*>(std::to_string(m_id).c_str()));
	m_graph->add_node(AGID(n), this);
}

Node::
Node(Graph* graph,
     Agnode_t* n)
	: m_graph(graph), n(n)
{
	m_id = AGID(n);
	m_graph->add_node(AGID(n), this);
}

}
