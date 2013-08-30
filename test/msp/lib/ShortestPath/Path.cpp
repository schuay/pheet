/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Path.h"

#include <assert.h>
#include <stdio.h>

using namespace graph;
using std::vector;

namespace sp
{

Path::
Path(graph::Node const* init)
	: m_tail(init), m_head(init)
{
	m_weight.resize(init->graph()->degree(), 0);
}

Path*
Path::
step(Edge const* edge) const
{
	graph::weight_vector_t ws = edge->weights();
	assert(ws.size() == m_weight.size());

	Path* p = new Path(*this);

	p->m_head = edge->head();
	p->m_edges.push_back(edge);

	for (size_t i = 0; i < ws.size(); i++) {
		p->m_weight[i] += ws[i];
	}

	return p;
}

void
Path::
print() const
{
	printf("%lu", m_tail->id());
	for (auto const e : m_edges) {
		printf(" -> %lu", e->head()->id());
	}
	printf(" (");
	for (auto const & w : m_weight) {
		printf("%d ", w);
	}
	printf(")\n");
}

Node const*
Path::
tail() const
{
	return m_tail;
}

Node const*
Path::
head() const
{
	return m_head;
}

vector<Edge const*>
Path::
edges() const
{
	return m_edges;
}

vector<weight_t>
Path::
weight() const
{
	return m_weight;
}

}
