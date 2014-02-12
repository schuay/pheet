/*
 * Copyright Jakob Gruber, Martin Kalany 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#ifndef VIRTUAL_ARRAY_H_
#define VIRTUAL_ARRAY_H_

#include <iterator>

#include "VirtualArrayBlock.h"

#define DATA_BLOCK_SIZE (100)

namespace pheet
{

/**
 * A virtual array of infinite size
 */
template <class T>
class VirtualArray
{
public:
	typedef VirtualArrayBlock<T, DATA_BLOCK_SIZE> Block;

	class VirtualArrayIterator : public std::iterator<std::random_access_iterator_tag, T>
	{
		friend class VirtualArray<T>;
	public:
		VirtualArrayIterator() : m_block(nullptr), m_block_nr(0), m_idx(0)
		{
		}

		VirtualArrayIterator(const VirtualArrayIterator& that)
			: m_block(that.m_block), m_block_nr(that.m_block_nr), m_idx(that.m_idx)
		{
		}

		VirtualArrayIterator& operator=(const VirtualArrayIterator& that)
		{
			m_block = that.m_block;
			m_block_nr = that.m_block_nr;
			m_idx = that.m_idx;
			return *this;
		}

		VirtualArrayIterator operator++(int)
		{
			if (m_block == nullptr) {
				return *this;
			}

			VirtualArrayIterator orig(*this);

			m_idx++;
			if (m_idx >= m_block->capacity()) {
				m_block = m_block->next;
				m_block_nr++;
				m_idx = 0;
			}

			return orig;
		}

		VirtualArrayIterator& operator--()
		{
			if (m_block == nullptr) {
				return *this;
			}

			if (m_idx == 0) {
				m_block = m_block->prev;
				m_block_nr--;
				m_idx = (m_block == nullptr) ? 0 : m_block->capacity() - 1;
			} else {
				m_idx--;
			}

			return *this;
		}

		T& operator*() const
		{
			return m_block->operator [](m_idx);
		}

		bool operator==(const VirtualArrayIterator& that)
		{
			return (m_block == that.m_block && m_idx == that.m_idx);
		}

		bool operator!=(const VirtualArrayIterator& that)
		{
			return !operator ==(that);
		}

		bool operator<(const VirtualArrayIterator& that)
		{
			if (m_block_nr < that.m_block_nr) {
				return true;
			}
			if (m_block_nr > that.m_block_nr) {
				return false;
			}
			return (m_idx < that.m_idx);
		}

//		friend void swap(iterator& lhs, iterator& rhs);

	private:
		VirtualArray::Block* m_block;
		ssize_t m_block_nr;	/**< The nr of the current block within the ordered sequence. */
		size_t m_idx;
	};

	VirtualArray()
		: m_block_cnt(1)
	{
		m_last = m_first = new Block();
	}

	~VirtualArray()
	{
		while (m_first->next != nullptr) {
			m_first = m_first->next;
			delete m_first->prev;
		}
		delete m_first;
	}

	VirtualArrayIterator begin() const
	{
		VirtualArrayIterator it;
		it.m_block = m_first;
	}

	VirtualArrayIterator iterator_to(const size_t idx) const
	{
		if (idx >= size()) {
			return end();
		}

		Block* block = find_block(idx);

		VirtualArrayIterator it;
		it.m_block = block;
		it.m_idx = idx % block_size();

		for (const Block* i = m_first; i != block; i = i->next) {
			it.m_block_nr++;
		}

		return it;
	}

	VirtualArrayIterator end() const
	{
		VirtualArrayIterator it;
		it.m_block_nr = m_block_cnt;
		return it;
	}

	void push(T item)
	{
		if (m_last->size() == m_last->capacity()) {
			add_block();
		}
		m_last->push(item);
	}

	//TODO: implement element access using iterators instead?
	T& operator[](size_t idx)
	{
		pheet_assert(idx < size());

		Block* block = find_block(idx);
		return (*block)[idx % block_size()];
	}

	void swap(size_t left, size_t right)
	{
		//TODO: tmp should be the last element of a block (so that spy finds the
		//element currently swapped out)
		auto& l = operator [](left);
		auto& r = operator [](right);

		/* This is really a case for std::swap, but apparently some magic will be
		 * going on here later. */
		const T tmp = l;
		l = r;
		r = tmp;
	}

	constexpr size_t block_size() const
	{
		return DATA_BLOCK_SIZE;
	}

private:
	size_t size() const
	{
		return DATA_BLOCK_SIZE * (m_block_cnt - 1) + m_last->size();
	}

	Block* find_block(size_t idx) const
	{
		//find block that stores element at location idx
		Block* tmp = m_first;
		size_t cnt = block_size();
		//TODO: reduce asymptotic complexity
		while (cnt <= idx) {
			tmp = tmp->next;
			cnt += block_size();
		}
		return tmp;
	}

	void add_block()
	{
		Block* tmp = new Block();
		tmp->prev = m_last;
		m_last->next = tmp;
		m_last = tmp;
		++m_block_cnt;
	}

private:
	Block* m_first;
	Block* m_last;
	size_t m_block_cnt;

};

} /* namespace pheet */

#endif /* VIRTUAL_ARRAY_H_ */
