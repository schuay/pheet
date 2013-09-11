/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "KDTree.h"

using namespace sp;

namespace pareto
{

struct tree_t {
	const PathPtr p;
	tree_t* l;
	tree_t* r;
};


bool
KDTree::
dominated(const sp::PathPtr path) const
{
	(void)path;
	return false;
}

void
KDTree::
prune(const sp::PathPtr path,
      sp::Paths& pruned)
{
	(void)path;
	(void)pruned;
}

void
KDTree::
insert(const sp::PathPtr path)
{
	(void)path;
}

}
