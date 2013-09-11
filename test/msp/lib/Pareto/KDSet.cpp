/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "KDSet.h"

using namespace graph;
using namespace sp;

namespace pareto
{

void
KDSet::
insert(PathPtr& path,
       Paths& added,
       Paths& removed)
{
	if (t.dominated(path)) {
		return;
	}

	t.prune(path, removed);
	t.insert(path);

	added.push_back(path);
}

Paths
KDSet::
paths() const
{
	return t.items();
}

}
