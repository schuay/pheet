/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "PriorityLinkedQueue.h"

#include <assert.h>
#include <queue>

using namespace sp;

namespace pareto
{

bool
PriorityLinkedQueue::elem_lexic_greater::
operator()(const elem_t* lhs, const elem_t* rhs) const
{
	assert(lhs && rhs);

	const std::vector<graph::weight_t> l = lhs->path->weight();
	const std::vector<graph::weight_t> r = rhs->path->weight();

	assert(l.size() == r.size());
	const int dims = l.size();

	for (int i = 0; i < dims; i++) {
		if (l[i] == r[i]) {
			continue;
		}
		return (l[i] > r[i]);
	}

	return false;
}

std::vector<PathPtr>
PriorityLinkedQueue::
first(const size_t n)
{
	assert(n == 1);

	std::priority_queue<elem_t*, std::vector<elem_t*>, elem_lexic_greater> q;

	for (elem_t* n = m_list; n != nullptr; n = n->next) {
		q.push(n);
	}

	std::vector<PathPtr> ps;
	for (size_t i = 0; i < n && !q.empty(); i++) {
		elem_t* n = q.top();
		q.pop();

		ps.push_back(n->path);

		m_elems_by_head[n->path->head()].erase(n);
		list_erase(n);
	}

	return ps;
}

}
