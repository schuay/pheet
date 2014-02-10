/*
 * Copyright Jakob Gruber, Martin Kalany 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#ifndef PIVOT_QUEUE_H_
#define PIVOT_QUEUE_H_

#include "PivotElement.h"

#include <vector>

namespace pheet
{

class PivotQueue
{
public:

	/**
	 * Put the PivotElement into this PivotQueue iff no equal PivotElement is in
	 * the queue.
	 */
	bool try_put(PivotElement* element)
	{
		for (size_t i = 0; i < size(); i++) {
			//check dimension first
			//TODO: assert val is not bigger than older pivots
			//TODO: complexity?
			PivotElement* other = elements[size() - i - 1];
			if (element->dimension() == other->dimension()) {
				if (element->value() == other->value()) {
					return false;
				} else {
					put(element);
					return true;
				}
			}
		}
		put(element);
		return true;
	}

	size_t refcnt(size_t idx) const
	{
		return elements[idx]->refcnt();
	}

	PivotElement* get(size_t idx)
	{
		pheet_assert(idx < elements.size());
		elements[idx]->increment_refcnt();
		return elements[idx];
	}

	void release(PivotElement* pivot)
	{
		size_t idx = pivot->pos();
		pheet_assert(idx < elements.size());
		pivot->decrement_refcnt();
		if (pivot->refcnt() == 0) {
			PivotElement* element = elements.back();
			elements.pop_back();
			delete element;
		}
	}

	size_t size() const
	{
		return elements.size();
	}

private:
	void put(PivotElement* element)
	{
		element->increment_refcnt();
		elements.push_back(element);
	}

private:
	std::vector<PivotElement*> elements;
};

} /* namespace pheet */

#endif /* PIVOT_QUEUE_H_ */
