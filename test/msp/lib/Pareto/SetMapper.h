/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __NODE_LOCAL_SET_H
#define __NODE_LOCAL_SET_H

#include "test/msp/lib/Graph/Graph.h"
#include <unordered_map>
#include "LockedSet.h"

namespace pareto
{
class SetMapper
{
public:

	SetMapper(graph::Graph const& g);
	~SetMapper();
	/*
	 * For p <- paths, insert e into the pareto set attached to p->head()
	 *
	 * Return all p <- paths
	 */
	sp::Paths insert(sp::Paths& paths);

private:
	std::unordered_map<const graph::Node*, LockedSet*> map;

};
}
#endif /*  __NODE_LOCAL_SET_H*/
