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
	 * Attempts to inserts path into the set.
	 *
	 * If path has been added successfully, added contains path on return. Otherwise
	 * added.empty() == true.
	 * removed contains all paths found to be dominated by path. 1) If path was added,
	 * these are all paths currently in the set dominated by path. 2) Otherwise,
	 * removed will only contain paths found to be dominated by path until a path
	 * that dominates path was found (i.e., a subset of case 1).
	 *
	 */
	virtual void insert(sp::PathPtr& path,
	                    sp::Paths& added,
	                    sp::Paths& removed) = 0;

	virtual sp::Paths paths() const = 0;

	virtual ~Set() { }
};

}

#endif /* __SET_H */
