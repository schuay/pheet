/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "PriorityQueue.h"

using namespace sp;

namespace pareto
{

bool
PriorityQueue::path_lincomb_greater::
operator()(sp::PathPtr const& lhs,
           sp::PathPtr const& rhs) const
{
	const graph::weight_vector_t& this_weight = lhs->weight();
	const graph::weight_vector_t& that_weight = rhs->weight();

	double this_priority = 0.0;
	for (size_t i = 0; i < this_weight.size(); i++) {
		this_priority += this_weight[i];
	}

	double that_priority = 0.0;
	for (size_t i = 0; i < that_weight.size(); i++) {
		that_priority += that_weight[i];
	}

	return (this_priority > that_priority);
}

PathPtr
PriorityQueue::
first()
{
	PathPtr p = nullptr;

	while (!p || p->dominated()) {
		if (m_queue.empty()) {
			return PathPtr();
		}
		p = m_queue.top();
		m_queue.pop();
	}

	return p;
}

void
PriorityQueue::
insert(PathPtr path)
{
	m_queue.push(path);
}

bool
PriorityQueue::
empty()
{
	while (!m_queue.empty() && m_queue.top()->dominated()) {
		m_queue.pop();
	}
	return m_queue.empty();
}

}
