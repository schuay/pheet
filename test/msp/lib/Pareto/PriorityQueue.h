/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __PRIORITY_QUEUE_H
#define __PRIORITY_QUEUE_H

#include "Queue.h"

#include <queue>

namespace pareto
{

/**
 * An implementation of the pareto queue using a priority queue and lazy removal.
 */
class PriorityQueue : public Queue
{
public:
	PriorityQueue();
	virtual ~PriorityQueue();

	sp::PathPtr first() override;
	void insert(sp::PathPtr path) override;
	bool empty() const override;

private:
	std::priority_queue<sp::PathPtr> m_queue;
};

}

#endif /* __PRIORITY_QUEUE_H */
