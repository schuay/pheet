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
Path(graph::Node const* init,
     const size_t degree)
	: m_tail(init), m_head(init), m_dominated(false)
{
    m_weight.resize(degree, 0);
}

Path::
Path(const Path& that)
	: m_tail(that.m_tail),
	  m_head(that.m_head),
	  m_edges(that.m_edges),
	  m_weight(that.m_weight),
	  m_dominated(false)
{
}

Path*
Path::
step(Edge const* edge) const
{
    graph::weight_vector_t const& ws = edge->weights;
    assert(ws.size() == m_weight.size());

	Path* p = new Path(*this);

    p->m_head = edge->head;
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

vector<Edge const*> const&
Path::
edges() const
{
	return m_edges;
}

weight_vector_t const&
Path::
weight() const
{
	return m_weight;
}

bool
Path::
dominated() const
{
	return m_dominated.load(std::memory_order_acquire);
}

void
Path::
set_dominated()
{
	m_dominated.store(true, std::memory_order_release);
}

}
