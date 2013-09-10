/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "KDSet.h"

#include <limits>

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
	assert(Dims == path->degree());

	std::vector<weight_t> mins(Dims, std::numeric_limits<weight_t>::min());
	std::vector<weight_t> maxs(Dims, std::numeric_limits<weight_t>::max());

	typename tree_t::_Region_ dominator_region(&mins[0], &path->weight()[0], t->value_acc(),
	        t->value_comp());
	if (t->count_within_range(dominator_region) > 0) {
		return;
	}

	typename tree_t::_Region_ dominatee_region(&path->weight()[0], &maxs[0], t->value_acc(),
	        t->value_comp());
	std::vector<PathPtr> dominatees;
	t->find_within_range(dominatee_region, std::back_inserter(dominatees));

	for (PathPtr const & p : dominatees) {
		t->erase_exact(p);
		removed.push_back(p);
	}

	t->insert(path);
	added.push_back(path);
}

template <int Dims>
Paths
KDSet<Dims>::
paths() const
{
	return sp::Paths(t->begin(), t->end());
}

template class KDSet<3>;

}
