/*
 * Copyright Jakob Gruber, Martin Kalany 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#ifndef DATA_BLOCK_H_
#define DATA_BLOCK_H_

#include "pheet/misc/assert.h"

namespace pheet
{

template <class T, size_t N>
class VirtualArrayBlock
{
private:
	typedef VirtualArrayBlock<T, N> Self;

public:
	VirtualArrayBlock<T, N>()
		: next(nullptr), prev(nullptr), m_data {nullptr}, m_size(0)
	{
	}

	void push(T item)
	{
		pheet_assert(m_size < N);
		m_data[m_size++] = item;
	}

	T pop()
	{
		pheet_assert(m_size > 0);
		return m_data[--m_size];
	}

	T& operator[](const size_t idx)
	{
		pheet_assert(idx < N);
		return m_data[idx];
	}

	size_t size() const
	{
		return m_size;
	}

	constexpr size_t capacity() const
	{
		return N;
	}

public:
	Self* next;
	Self* prev;

private:
	T m_data[N];
	size_t m_size;

};

} /* namespace pheet */

#endif /* DATA_BLOCK_H_ */
