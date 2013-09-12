/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "SortedSet.h"

#include <algorithm>

using namespace graph;
using namespace sp;

namespace pareto
{

bool
SortedSet::lincomb_less::
operator()(PathPtr const& l,
           PathPtr const& r) const
{
	const weight_t lsum = std::accumulate(l->weight().begin(), l->weight().end(), 0);
	const weight_t rsum = std::accumulate(r->weight().begin(), r->weight().end(), 0);

	return (lsum < rsum);
}

void
SortedSet::
insert(PathPtr& path,
       Paths& added,
       Paths& removed)
{
	const auto upper_bound = m_set.upper_bound(path);
	for (auto it = m_set.begin(); it != upper_bound; it++) {
		if (dominates(it->get(), path.get())) {
			return;
		}
	}

	for (auto it = upper_bound; it != m_set.end();) {
		if (dominates(path.get(), it->get())) {
			removed.push_back(*it);
			m_set.erase(it++);
		} else {
			it++;
		}
	}

	added.push_back(path);
	m_set.insert(path);
}

sp::Paths
SortedSet::
paths() const
{
	return sp::Paths(m_set.begin(), m_set.end());
}

}
