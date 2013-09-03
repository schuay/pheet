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
	sp::Paths added, removed;
	for (auto p : paths) {
		insert(p, added, removed);
	}
	return added;
}

void
NaiveSet::
insert(sp::PathPtr& path,
       sp::Paths& added,
       sp::Paths& removed)
{
	less dominates;

	for (auto i = m_set.begin(); i != m_set.end();) {
		PathPtr rhs = *i;
		if (dominates(path.get(), rhs.get())) {
			removed.push_back(rhs);
			m_set.erase(i++);
		} else if (dominates(rhs.get(), path.get())) {
			return;
		} else {
			i++;
		}
	}

	added.push_back(path);
	m_set.insert(path);
}

}
