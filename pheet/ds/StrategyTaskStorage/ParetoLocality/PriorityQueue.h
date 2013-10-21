/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include <queue>

namespace pheet
{

template <class Pheet, class Item>
class PriorityQueue
{
public:
	Item* first();
	void insert(Item* item);
	bool empty();

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


template <class Pheet, class Item>
Item*
PriorityQueue<Pheet, Item>::
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

template <class Pheet, class Item>
void
PriorityQueue<Pheet, Item>::
insert(Item* item)
{
	queue.push(item);
}

template <class Pheet, class Item>
bool
PriorityQueue<Pheet, Item>::
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
