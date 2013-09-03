/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "LockedSet.h"

namespace pareto
{

sp::Paths
LockedSet::
insert(sp::Paths& paths)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return NaiveSet::insert(paths);
}

void
LockedSet::
insert(sp::PathPtr& path,
       sp::Paths& added,
       sp::Paths& removed)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	NaiveSet::insert(path, added, removed);

}

}
