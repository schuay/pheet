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
Path()
	: m_tail(nullptr),
	  m_head(nullptr),
	  m_pred(nullptr),
	  m_weight(),
	  m_weight_sum(0),
	  m_degree(0),
	  m_dominated(false)
{
}

Path::
Path(Path const& that)
	: m_tail(that.m_tail),
	  m_head(that.m_head),
	  m_pred(nullptr),
	  m_weight(that.m_weight),
	  m_weight_sum(that.m_weight_sum),
	  m_degree(that.m_degree),
	  m_dominated(that.m_dominated.load())
{
}

void
Path::
step(Edge const* edge, Path const* path)
{
	graph::weight_vector_t const& ws = edge->weights();
	assert(ws.size() == path->m_weight.size());

	m_tail = path->m_tail;
	m_head = edge->head();
	m_pred = path;
	m_weight = path->m_weight;
	m_weight_sum = path->m_weight_sum;
	m_degree = path->m_degree;
	m_dominated = false;

	for (size_t i = 0; i < ws.size(); i++) {
		m_weight[i] += ws[i];
		m_weight_sum += ws[i];
	}
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

void
Path::
print_verbose() const
{
	if (m_pred != nullptr) {
		m_pred->print_verbose();
		printf("\t-- (");
		for (auto const & w : m_weight) {
			printf("%d ", w);
		}
		printf(")");
		printf("-> %lu\n",  m_head->id());
	} else {
		printf("%lu\n",  m_head->id());
	}
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
