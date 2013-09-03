/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "LockedSet.h"
#include "SetMapper.h"

namespace pareto
{

SetMapper::
SetMapper(graph::Graph const* g)
{
	for (auto node : g->nodes()) {
		Set* set = new LockedSet();
		map.insert(std::pair<graph::Node const*, Set*>(node, set));
	}
}

SetMapper::
~SetMapper()
{
	for (auto it = map.begin(); it != map.end(); ++it) {
		delete it->second;
	}

}

void
SetMapper::
insert(sp::Paths& paths,
       sp::Paths& added,
       sp::Paths&)
{
	for (auto p : paths) {
		graph::Node const* node = p->head();
		if (map[node]->insert(p)) {
			added.push_back(p);
		}
	}
}

}
