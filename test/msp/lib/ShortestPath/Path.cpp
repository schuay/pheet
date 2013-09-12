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
	: m_tail(init),
	  m_head(init),
	  m_pred(nullptr),
	  m_weight_sum(0),
	  m_degree(init->graph()->degree()),
	  m_dominated(false)
{
	m_weight.resize(m_degree, 0);
}

Path::
Path(const Path& that)
	: m_tail(that.m_tail),
	  m_head(that.m_head),
	  m_pred(that.m_pred),
	  m_weight(that.m_weight),
	  m_weight_sum(that.m_weight_sum),
	  m_degree(that.m_degree),
	  m_dominated(false)
{
}

Path*
Path::
step(Edge const* edge) const
{
	graph::weight_vector_t const& ws = edge->weights();
	assert(ws.size() == m_weight.size());

	Path* p = new Path(*this);

	p->m_head = edge->head();
	p->m_pred = this;

	for (size_t i = 0; i < ws.size(); i++) {
		p->m_weight[i] += ws[i];
		p->m_weight_sum += ws[i];
	}

	return p;
}

void
Path::
print() const
{
	printf("%lu --> %lu", m_tail->id(), m_head->id());
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
