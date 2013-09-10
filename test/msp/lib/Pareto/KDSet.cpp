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

template <int Dims>
KDSet<Dims>::
KDSet()
{
	t = new tree_t();
}

template <int Dims>
KDSet<Dims>::
~KDSet()
{
	delete t;
}

template <int Dims>
void
KDSet<Dims>::
insert(PathPtr& path,
       Paths& added,
       Paths& removed)
{
	(void)path;
	(void)added;
	(void)removed;
}

template <int Dims>
Paths
KDSet<Dims>::
paths() const
{
	return sp::Paths(t->begin(), t->end());
}

}
