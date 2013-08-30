/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __PATH_H
#define __PATH_H

#include <memory>
#include <vector>

#include "test/msp/lib/Graph/Edge.h"

namespace sp
{

class Path;
typedef std::shared_ptr<Path> PathPtr;
typedef std::vector<PathPtr> Paths;

class Path
{
public:
	Path(graph::Node const* init);

	/** Returns a new path object which represents the path generated
	 *  by appending the given edge to the current path. */
	Path* step(graph::Edge const* edge) const;

	void print() const;

	graph::Node const* tail() const;
	graph::Node const* head() const;
	std::vector<graph::Edge const*> edges() const;
	std::vector<graph::weight_t> weight() const;

private:
	graph::Node const* m_tail;
	graph::Node const* m_head;
	std::vector<graph::Edge const*> m_edges;
	std::vector<graph::weight_t> m_weight;
};

}

#endif /* __PATH_H */
