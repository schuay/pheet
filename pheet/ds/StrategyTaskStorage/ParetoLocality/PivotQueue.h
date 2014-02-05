#ifndef PIVOT_QUEUE_H_
#define PIVOT_QUEUE_H_

#include "PivotElement.h"

#include <assert.h>
#include <vector>

class PivotQueue
{
public:

	void put(PivotElement* element) {
		//TODO: shall not contain equal elements
		element->increment_refcnt();
		elements.push_back(element);
	}

	/**
	 * Put the PivotElement into this PivotQueue iff no equal PivotElement is in
	 * the queue.
	 */
	bool try_put(PivotElement* element) {
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

	size_t refcnt(size_t idx) {
		return elements[idx]->refcnt();
	}

	PivotElement* get(size_t idx) {
		assert(idx < elements.size());
		elements[idx]->increment_refcnt();
		return elements[idx];
	}

	void release(size_t idx) {
		assert(idx < elements.size());
		elements[idx]->decrement_refcnt();
		assert(elements[idx]->refcnt() >= 0);
		if (elements[idx]->refcnt() == 0) {
			//assert(idx == elements.size() - 1);
			PivotElement* element = elements.back();
			elements.pop_back();
			delete element;
		}
	}

	size_t size() {
		return elements.size();
	}

private:
	std::vector<PivotElement*> elements;
};

#endif /* PIVOT_QUEUE_H_ */
