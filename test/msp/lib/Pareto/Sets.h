/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __SETS_H
#define __SETS_H

#include <unordered_map>

#include "Set.h"
#include "test/msp/lib/Graph/Graph.h"

namespace pareto
{

class Sets
{
public:

	Sets(graph::Graph const* g);
	virtual ~Sets();

	/**
	 * For p <- paths, insert e into the pareto set attached to p->head()
	 *
	 * After execution, the added vector contains all p <- paths which have been
	 * added (= which were not dominated).
	 * Likewise, the removed vector contains all p <- this which were removed from
	 * the current Sets. Note that in the current implementation, the intersection
	 * of removed and added paths may be non-empty.
	 */
	void insert(sp::Paths& paths,
	            sp::Paths& added,
	            sp::Paths& removed);

	sp::ShortestPaths* shortest_paths() const;

private:
	std::unordered_map<graph::Node const*, Set*> map;

};

}

#endif /*  __SETS_H*/
