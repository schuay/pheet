/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __QUEUE_H
#define __QUEUE_H

#include "test/msp/lib/ShortestPath/Path.h"

namespace pareto
{

/**
 * A high level interface for the required pareto queue operations
 * first() and insert().
 */
class Queue
{
public:
	/**
	 * Returns the first non-dominated element e and removes it from the queue.
	 */
	virtual sp::PathPtr first() = 0;

	/**
	 * Inserts path into the queue.
	 */
	virtual void insert(sp::PathPtr path) = 0;

	/**
	 * Returns true if the queue contains zero non-dominated items.
	 */
	virtual bool empty() = 0;
};

}

#endif /* __QUEUE_H */
