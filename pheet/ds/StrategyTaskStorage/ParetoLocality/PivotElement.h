/*
 * Copyright Jakob Gruber, Martin Kalany 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#ifndef PIVOT_ELEMENT_H_
#define PIVOT_ELEMENT_H_

#include <stdlib.h>

class PivotElement
{
public:
	PivotElement(size_t dimension, size_t value)
		: m_dim(dimension), m_val(value), m_refcnt(0) {}

	size_t value() const
	{
		return m_val;
	}

	size_t dimension() const
	{
		return m_dim;
	}

	size_t refcnt() const
	{
		return m_refcnt;
	}

	void increment_refcnt()
	{
		++m_refcnt;
	}

	void decrement_refcnt()
	{
		assert(m_refcnt > 0);
		--m_refcnt;
	}

	bool equal(PivotElement const* other) const
	{
		return (m_dim == other->dimension() &&  m_val == other->value());
	}

private:
	const size_t m_dim;
	const size_t m_val;
	size_t m_refcnt;
};

#endif /* PIVOT_ELEMENT_H_ */
