/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "LockedSet.h"

namespace pareto
{

void
LockedSet::
insert(sp::PathPtr& path,
       sp::Paths& added,
       sp::Paths& removed)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	SortedSet::insert(path, added, removed);

}

sp::Paths
LockedSet::
paths() const
{
	return SortedSet::paths();
}

}
