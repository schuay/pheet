/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __SET_H
#define __SET_H

#include "lib/ShortestPath/Path.h"
#include "lib/ShortestPath/ShortestPaths.h"

namespace pareto
{

/**
 * Interface for Pareto set operations.
 */
class Set
{
public:
	/**
	 * Attempts to inserts path into the set. Returns true, iff the path
	 * has been successfully inserted. Otherwise, the removed vector contains
	 * a list of paths which were dominated by path.
	 */
	virtual void insert(sp::PathPtr& path,
	                    sp::Paths& added,
	                    sp::Paths& removed) = 0;

	virtual sp::Paths paths() const = 0;

	virtual ~Set() { }

};

}

#endif /* __SET_H */
