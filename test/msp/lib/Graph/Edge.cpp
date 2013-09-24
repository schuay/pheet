/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Edge.h"

#include <assert.h>
#include <graphviz/cgraph.h>
#include <sstream>

namespace graph
{

Edge::
Edge(Node* t,
     Node* h,
     weight_vector_t const& ws)
	: t(t), h(h), ws(ws)
{
	assert(t->graph() == h->graph());
	assert(ws.size() == t->graph()->degree());

	m_graph = t->graph();
	e = agedge(m_graph->g, t->n, h->n, NULL, TRUE);
	m_graph->add_edge(this);

	std::stringstream ss;
	for (auto i : ws) {
		ss << i << " ";
	}
	agset(e, ATTR_WEIGHT, const_cast<char*>(ss.str().c_str()));
}

Edge::
Edge(Graph* graph,
     Agedge_t* e)
	: m_graph(graph), e(e)
{
	t = m_graph->m_nodes_map[agtail(e)];
	h = m_graph->m_nodes_map[aghead(e)];

	std::stringstream ss(std::string(agget(e, ATTR_WEIGHT)));
	weight_t w;
	while (ss >> w) {
		ws.push_back(w);
	}

	assert(ws.size() == m_graph->degree());

	m_graph->add_edge(this);
}

Graph*
Edge::
graph() const
{
	return m_graph;
}

Node*
Edge::
tail() const
{
	return t;
}

Node*
Edge::
head() const
{
	return h;
}

weight_vector_t const&
Edge::
weights() const
{
	return ws;
}

}
