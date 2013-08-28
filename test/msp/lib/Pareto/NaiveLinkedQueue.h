/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __NAIVE_LINKED_QUEUE_H
#define __NAIVE_LINKED_QUEUE_H

#include "LinkedQueue.h"

namespace pareto
{

/**
 * A variant of the linked pareto queue which uses a naive nested loop
 * algorithm to return multiple Pareto optimal elements from first().
 */
class NaiveLinkedQueue : public LinkedQueue
{
public:
	std::vector<sp::PathPtr> first(const size_t n) override;
};

}

#endif /* __NAIVE_LINKED_QUEUE_H */
