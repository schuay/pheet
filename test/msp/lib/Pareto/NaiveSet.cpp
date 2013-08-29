/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Less.h"
#include "NaiveSet.h"

using namespace sp;

namespace pareto
{

sp::Paths
NaiveSet::
insert(sp::Paths& paths)
{
	sp::Paths non_dominated;
	for (auto p : paths) {
		if (insert(p)) {
			non_dominated.push_back(p);
		}
	}
	return non_dominated;
}

bool
NaiveSet::
insert(sp::PathPtr& path)
{
	less dominates;
	bool add_elem = true;
	PathPtr lhs = path;
	for (auto i = m_set.begin(); i != m_set.end();) {
		PathPtr rhs = *i;
		if (dominates(lhs.get(), rhs.get())) {
			/* TODO: Additional set requirement:
			 * * A way of marking pruned tasks as dead. We need to find out if we can
			 *   actually (safely) access the strategy object after insertion into the
			 *   scheduler, or if we should do it implicitly (like Sssp).
			 */
			m_set.erase(i++);
		} else if (dominates(rhs.get(), lhs.get())) {
			add_elem = false;
			break;
		} else {
			++ i;
		}
	}
	if (add_elem) {
		m_set.insert(path);
	}
	return add_elem;
}
}
