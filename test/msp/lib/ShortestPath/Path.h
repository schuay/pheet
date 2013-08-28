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
	Path(const graph::Node* init);

	/** Returns a new path object which represents the path generated
	 *  by appending the given edge to the current path. */
	Path* step(const graph::Edge* edge) const;

	void print() const;

	const graph::Node* tail() const;
	const graph::Node* head() const;
	std::vector<const graph::Edge*> edges() const;
	std::vector<graph::weight_t> weight() const;

private:
	const graph::Node* m_tail;
	const graph::Node* m_head;
	std::vector<const graph::Edge*> m_edges;
	std::vector<graph::weight_t> m_weight;
};

}

#endif /* __PATH_H */
