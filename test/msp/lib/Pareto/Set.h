/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __SET_H
#define __SET_H

#include "test/msp/lib/ShortestPath/Path.h"

namespace pareto
{

/**
 * Interface for Pareto set operations.
 */
class Set
{
public:

	/**
	 * DEPRECATED.
	 *
	 * Insert the set of elements into the pareto set.
	 *
	 * For all elements e in paths:
	 * 1) If e is dominated within its current node set, do nothing.
	 * 2) Otherwise, add e and prune all dominated elements of the node set.
	 *
	 * Return the set of inserted paths.
	 */
	virtual sp::Paths insert(sp::Paths& paths) = 0;

	/**
	 * Attempts to inserts path into the set. Returns true, iff the path
	 * has been successfully inserted. Otherwise, the removed vector contains
	 * a list of paths which were dominated by path.
	 */
	virtual void insert(sp::PathPtr& path,
	                    sp::Paths& added,
	                    sp::Paths& removed) = 0;

	virtual ~Set() { }
};

}

#endif /* __SET_H */
