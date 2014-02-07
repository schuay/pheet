/*
 * Copyright Jakob Gruber, Martin Kalany 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#ifndef PARTITION_POINTERS_H_
#define PARTITION_POINTERS_H_

#include <vector>
#include "PivotQueue.h"

class PartitionPointers
{
public:
	PartitionPointers(PivotQueue* pivot_queue, size_t block_size, size_t last_element = 0)
		: m_pivot_queue(pivot_queue), m_last(0), m_dead(block_size), m_end(last_element)
	{
		m_idx.push_back(std::make_pair(0, nullptr));
	}

	~PartitionPointers()
	{
		while (m_idx.size() > 1) {
			m_pivot_queue->release(m_idx.back().second);
			m_idx.pop_back();
		}
	}

	bool fall_back()
	{
		assert(m_pivot_queue->size() >= m_idx.size() - 1);
		if (m_idx.size() == 1) {
			return false;
		}
		m_dead = m_last;
		//reduce reference count on pivot element used for that partition step
		//Note: first partition pointer is always index 0 and is not associated
		//with a pivot element
		m_pivot_queue->release(m_idx.back().second);
		//remove the partition pointer
		m_idx.pop_back();
		assert(m_idx.size() > 0);
		m_last = m_idx.back().first;

		return true;
	}

	size_t size() const
	{
		return m_idx.size();
	}

	size_t last() const
	{
		return m_last;
	}

	//TODO: dead is not a good name, since it is also used for dead tasks
	size_t dead() const
	{
		return m_dead;
	}

	void dead(size_t val)
	{
		m_dead = val;
	}

	void decrease_dead()
	{
		m_dead--;
	}

	size_t end() const
	{
		return m_end;
	}

	void increment_end()
	{
		++m_end ;
	}

	void add(size_t idx, PivotElement* pivot)
	{
		m_idx.push_back(std::make_pair(idx, pivot));
		assert(m_pivot_queue->refcnt(m_idx.size() - 2) > 0);
		m_last = idx;
	}

private:
	PivotQueue* m_pivot_queue;
	std::vector<std::pair<size_t, PivotElement*>> m_idx;
	/* start of last partition (excluding dead items) */
	size_t m_last;
	/* Dead items are stored right of the right-most partition, i.e., at the
	 * very end */
	size_t m_dead;
	/* end of block. Points to (last+1) item, i.e., last item is at end-1 */
	size_t m_end;

};

#endif /* PARTITION_POINTERS_H_ */
