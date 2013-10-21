/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include "ParetoQueue.h"
#include <queue>

namespace pheet
{

template <class Item>
class PriorityQueue : public ParetoQueue<Item>
{
public:
	/**
	 * Returns the first non-dominated element e and removes it from the queue.
	 *
	 * Dominated elements encoutered while searching for e are removed from
	 * the queue. If no such item is in the queue, the nullptr is returned.
	 *
	 */
	Item* first() override;


	void insert(Item* item) override;

	/**
	 * Returns true if the queue contains zero non-dominated items.
	 *
	 * All dominated items are removed from the queue.
	 */
	bool empty() override;

private:
	class comp
	{
	public:
		bool operator()(Item* lhs, Item* rhs) const {
			return rhs->strategy.prioritize(lhs->strategy);
		}
	};

private:
	std::priority_queue<Item*, std::vector<Item*>, comp> queue;
};


template <class Item>
Item*
PriorityQueue<Item>::
first()
{
	Item* item;
	if (queue.empty()) {
		return nullptr;
	}
	item = queue.top();
	queue.pop();
	return item;
}

template <class Item>
void
PriorityQueue<Item>::
insert(Item* item)
{
	queue.push(item);
}

template <class Item>
bool
PriorityQueue<Item>::
empty()
{
	while (!queue.empty() && queue.top()->strategy.dead_task()) {
		queue.top()->take_and_delete();
		queue.pop();
	}
	return queue.empty();
}

} /* namespace pheet */

#endif /* PRIORITYQUEUE_H_ */
