#ifndef PIVOT_ELEMENT_H_
#define PIVOT_ELEMENT_H_

#include <stdlib.h>

class PivotElement
{
public:
	PivotElement(size_t dimension, size_t value)
		: m_dim(dimension), m_val(value), m_refcnt(0) {}

	size_t value() {
		return m_val;
	}

	size_t dimension() {
		return m_dim;
	}

	size_t refcnt() {
		return m_refcnt;
	}

	void increment_refcnt() {
		++m_refcnt;
	}

	void decrement_refcnt() {
		--m_refcnt;
	}

	bool equal(PivotElement* other) {
		return (m_dim == other->dimension() &&  m_val == other->value());
	}

private:
	size_t m_dim;
	size_t m_val;
	size_t m_refcnt;
};

#endif /* PIVOT_ELEMENT_H_ */
