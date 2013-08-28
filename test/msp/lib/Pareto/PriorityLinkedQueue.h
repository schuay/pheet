/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __PRIORITY_LINKED_QUEUE_H
#define __PRIORITY_LINKED_QUEUE_H

#include "LinkedQueue.h"

namespace pareto
{

/**
 * A variant of the linked pareto queue using a priority queue
 * to determine elements returned by first().
 * It is more efficient than the naive version, but may only return a single
 * element from first() instead of n.
 */
class PriorityLinkedQueue : public LinkedQueue
{
public:
	std::vector<sp::PathPtr> first(const size_t n) override;

private:
	class elem_lexic_greater
	{
	public:
		bool operator()(const elem_t* lhs, const elem_t* rhs) const;
	};
};

}

#endif /* __PRIORITY_LINKED_QUEUE_H */
