/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Less.h"
#include "SortedSet.h"

using namespace sp;

namespace pareto
{

void
SortedSet::
insert(sp::PathPtr& path,
       sp::Paths& added,
       sp::Paths& removed)
{
}

sp::Paths
SortedSet::
paths() const
{
	return sp::Paths(m_set.begin(), m_set.end());
}

}
