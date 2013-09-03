/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __SET_MAPPER_H
#define __SET_MAPPER_H

#include <unordered_map>

#include "test/msp/lib/Graph/Graph.h"
#include "Set.h"

namespace pareto
{

class SetMapper
{
public:

	SetMapper(graph::Graph const* g);
	virtual ~SetMapper();

	/*
	 * For p <- paths, insert e into the pareto set attached to p->head()
	 *
	 * After execution, the added vector contains all p <- paths which have been
	 * added (= which were not dominated).
	 * Likewise, the removed vector contains all p <- this which were removed from
	 * the current SetMapper. Note that in the current implementation, the intersection
	 * of removed and paths may be non-empty.
	 */
	void insert(sp::Paths& paths,
	            sp::Paths& added,
	            sp::Paths& removed);

private:
	std::unordered_map<graph::Node const*, Set*> map;

};

}

#endif /*  __SET_MAPPER_H*/
