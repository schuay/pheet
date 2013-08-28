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
