/*
 * Copyright Jakob Gruber, Martin Kalany 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#ifndef PARETOLOCALITYTASKSTORAGEBLOCK_H_
#define PARETOLOCALITYTASKSTORAGEBLOCK_H_

#include "PartitionPointers.h"
#include "PivotQueue.h"
#include "VirtualArray.h"

#include <cmath>
#include <random>

/* Maximum number of attempts to
 * - generate a pivot element s.t. an equal element is not yet on the PivotQueue
 * - partition a block s.t. after partitioning the block, the right-most
 *   partition (excluding dead) contains at least 1 item
 */
#define MAX_ATTEMPTS (3)

namespace pheet
{

template<class Item, size_t MAX_PARTITION_SIZE>
class ParetoLocalityTaskStorageBlock
{
public:
	typedef typename Item::T T;


	ParetoLocalityTaskStorageBlock(VirtualArray<Item*>& array, size_t offset, PivotQueue* pivots,
	                               size_t lvl = 0)
		: m_data(array), m_offset(offset), m_size(0), m_lvl(lvl), m_pivots(pivots)
	{
		m_capacity = MAX_PARTITION_SIZE * pow(2, m_lvl);
		m_partitions = new PartitionPointers(m_pivots, m_capacity);
	}

	~ParetoLocalityTaskStorageBlock()
	{
		delete m_partitions;
	}

	bool try_put(Item* item)
	{
		if (m_size == m_capacity) {
			return false;
		}
		put(item);
		return true;
	}

	void put(Item* item)
	{
		pheet_assert(m_size < m_capacity);
		//we only put data in a lvl 0 block
		pheet_assert(m_lvl == 0);
		m_data[m_size + m_offset] = item;
		m_partitions->increment_end();
		++m_size;
	}

	/**
	 * Return an item that is not dominated by any other item in this block.
	 *
	 * Do not remove this item from the block. If such an item does not exist,
	 * return nullptr.
	 *
	 * Any dead items that are inspected are cleaned up. Thus, if nullptr is
	 * returned, the block can be destructed.
	 */
	Item* top()
	{
		Item* best = nullptr;

		//iterate through items in right-most partition
		const size_t end = m_offset + std::min(m_partitions->end(), m_partitions->dead());
		auto it = m_data.iterator_to(m_offset + m_partitions->last());
		const auto end_it = m_data.iterator_to(end);
		for (; it != end_it; it++) {
			Item* item = *it;
			if (item == nullptr) {
				continue;
			}

			if (item->is_taken()) {
				continue;
			}

			if (item->is_dead()) {
				/* TODO: With multiple threads, this can lead to errors. */
				item->take_and_delete();
				/* TODO: we can not delete items, since another thread might
				 * access them. Use a MemoryManager that governs the use of
				 * Item instances */
				//delete item;
				*it = nullptr;
				continue;
			}

			if (best == nullptr || item->strategy()->prioritize(*best->strategy())) {
				best = item;
			}
		}

		//only happens if no more item that is not taken or dead is in current partition
		if (best == nullptr) {
			//fall back to previous partition, if possible
			if (m_partitions->fall_back()) {
				//call top() again, now operating on previous partition
				best = top();
			} else {
				/* We called top() on the first (and only) partition and did not
				 * get an active item back. Thus, we can clean up and free the
				 * block. Note that items in the dead partition have already
				 * been deleted during the last partition step. The block may
				 * contain dead or taken items in the range [0, min(dead, end)[
				 */
				clean_up();
			}
		}
		pheet_assert(!(best && best->is_taken_or_dead())); //don't return dead or taken items
		return best;
	}

	/**
	 * Take the given item and return its data.
	 *
	 * An item that is taken is marked for deletion/reuse and will not be returned
	 * via a call to top() anymore.
	 */
	T take(Item* item)
	{
		T data = item->take();
		/* TODO: we can delete item and set it's entry in the VirtualArray to
		 * nullptr. But for this we need the item's index here*/
		//delete item;
		//data_at(i) = nullptr; //This is pseudo code
		return data;
	}

	ParetoLocalityTaskStorageBlock* merge_next()
	{
		pheet_assert(m_next != nullptr);
		pheet_assert(m_next ->lvl() == m_lvl);
		//we only merge full blocks
		pheet_assert(m_size == m_capacity);

		//expand this block to cover this as well as next block
		++m_lvl;
		m_capacity <<= 1;
		//merging two full blocks of level lvl results in one full block of lvl+1
		m_size = m_capacity;

		//splice out next
		ParetoLocalityTaskStorageBlock* tmp  = m_next;
		if (tmp->next()) {
			tmp->next()->prev(this);
		}
		m_next = tmp->next();
		delete tmp;

		return this;
	}

	void partition()
	{
		delete m_partitions;
		m_partitions = new PartitionPointers(m_pivots, m_capacity, m_capacity);
		partition(0, 0, m_capacity - 1);
		drop_dead_items();
	}

	ParetoLocalityTaskStorageBlock* prev() const
	{
		return m_prev;
	}

	void prev(ParetoLocalityTaskStorageBlock* b)
	{
		m_prev = b;
	}

	ParetoLocalityTaskStorageBlock* next() const
	{
		return m_next;
	}

	void next(ParetoLocalityTaskStorageBlock* b)
	{
		m_next = b;
	}

	size_t lvl() const
	{
		return m_lvl;
	}

	size_t capacity() const
	{
		return m_capacity;
	}

	size_t offset() const
	{
		return m_offset;
	}

private:

	void clean_up()
	{
		size_t end = std::min(m_partitions->dead(), m_partitions->end());
		drop_dead_items(0, end);
	}

	/**
	 * Drop all items in the "dead tasks" partition
	 */
	void drop_dead_items()
	{
		drop_dead_items(m_partitions->dead(), m_capacity);
	}

	void drop_dead_items(size_t start, size_t end)
	{
		auto it = m_data.iterator_to(m_offset + start);
		const auto end_it = m_data.iterator_to(m_offset + end);
		for (; it != end_it; it++) {
			Item* item = *it;
			pheet_assert(!item || item->is_taken_or_dead());

			if (item && !item->is_taken()) {
				item->take_and_delete();
			}

			if (item) {
				/* TODO: we can not delete items, since another thread might
				 * access them. Use a MemoryManager that governs the use of
				 * Item instances */
				//delete item;
			}
			//TODO: make sure item is free'ed or reused
			*it = nullptr;
		}
	}

	void partition(size_t depth, size_t l /* TODO */, size_t r /* TODO */)
	{
		auto left = m_data.iterator_to(m_offset + l);
		auto right = m_data.iterator_to(m_offset + r);

		pheet_assert(left < right);
		const auto old_left = left;

		//generate new pivot element if neccesarry
		PivotElement* pivot;
		if (m_pivots->size() <= depth) {
			pivot = generate_pivot(l, r, depth);
			if (pivot == nullptr) {
				/* could not generate suitable pivot element -> Abort partitioning
				 * Right-most partition will exceed MAX_PARTITION_SIZE
				 */
				return;
			}
		} else {
			pivot = m_pivots->get(depth);
		}

		const size_t p_dim = pivot->dimension();
		const size_t p_val = pivot->value();

		do {
			//TODO: try to call is_taken_or_dead as little as possible (may be expensive, since user implemented)
			while (left < right && *left && !left->is_taken_or_dead()
			        && left->strategy()->less_priority(p_dim, p_val)) {
				left++;
			}

			while (left < right && *right && !right->is_taken_or_dead()
			        && (right->strategy()->greater_priority(p_dim, p_val)
			            || right->strategy()->equal_priority(p_dim, p_val))) {
				--right;
			}

			if (left != right) {
				if (!*right || right->is_taken_or_dead()) {
					//right is dead
					if (m_partitions->dead() - right.index() + m_offset == 1) {
						//element after right is dead too. Advance dead and right.
						//This is safe since left < right
						m_partitions->dead(right.index() - m_offset);
						pheet_assert(left < right);
						--right;
					} else {
						//swap right with rightmost non-dead element
						m_partitions->decrease_dead();
						auto dead = m_data.iterator_to(m_partitions->dead() + m_offset);
						swap(right, dead);
					}
				} else if (!*left || left->is_taken_or_dead()) {
					/* left is dead. Note that left+1==dead may never occur while
					 * left < right, since right < dead holds. */
					pheet_assert(left.index() - m_offset + 1 < m_partitions->dead());
					/* swap left with rightmost non-dead element. This may swap
					 * an element >=pivot to left, but we will not advance left.
					 * Progress is made by putting one dead element into it'S final
					 * place */
					m_partitions->decrease_dead();
					auto dead = m_data.iterator_to(m_partitions->dead() + m_offset);
					swap(left, dead);
					//if now right == dead, advance right
					if (dead == right) {
						--right;
					}
				} else {
					/* neither left nor right are dead. Swap left and right */
					swap(left, right);
					/* items at left and right are now at the correct position.
					 * We thus may advance both indices. However, in case we
					 * swapped when left + 1 == right, this will result in
					 * left == right + 1. */
					left++;
					--right;
				}
			}
		} while (left < right);

		/* Partitioning finished when left <= right. Left == right +1 is the case
		 * if the last swap was on indices s.t. left + 1 == right and both items
		 * were not dead. */
		pheet_assert(left == right || left.index() - m_offset == right.index() - m_offset + 1);

		//check if left points to dead item
		if (!*left || left->is_taken_or_dead()) {
			m_partitions->decrease_dead();
			auto dead = m_data.iterator_to(m_partitions->dead() + m_offset);
			if (left == dead) {
				--left;
			} else {
				swap(left, dead);
			}
		}
		pheet_assert(*left);

		//check if item at left belongs to left or right partition
		if (left->strategy()->greater_priority(p_dim, p_val)) {
			left++;
		}
		pheet_assert(left.index() - m_offset <= m_partitions->dead());

		//check if the last partitioning step needs to be redone
		if (!partition_failed(pivot, left.index() - m_offset)) {
			m_failed_attempts = 0;
		} else {
			/* partitioning failed. Reset the start of the section that needs
			 * to be partitioned further. Note: The end of this section is
			 * the start of the dead partition.
			 */
			left = old_left;
			/* If right-most partition > MAX_PARTITION_SIZE, we will partition again.
			 * This could potentially run indefinitely (think of all items having
			 * the same priority vector). Thus, bound the number of subsequent
			 * partitioning attempts that do not create a new partition.
			 */
			++m_failed_attempts;
		}

		if ((m_partitions->dead() - left.index() + m_offset > MAX_PARTITION_SIZE)
		        && m_failed_attempts < MAX_ATTEMPTS) {
			/* If partitioning succeeded but the resulting right-most (excluding
			 * dead) partition is >MAX_PARTITION_SIZE, partition recursively */
			if (m_failed_attempts == 0) {
				++depth;
			}
			partition(depth, left.index() - m_offset, m_partitions->dead() - 1);
		}
		m_failed_attempts = 0;
	}

	void swap(typename VirtualArray<Item*>::VirtualArrayIterator& lhs,
	          typename VirtualArray<Item*>::VirtualArrayIterator& rhs)
	{
		Item* item = *lhs;
		*lhs = *rhs;
		*rhs = item;
	}

	/**
	 *
	 * A partitioning step failed iff
	 * - The newly created right-most partition (excluding dead partition) is
	 *   empty; and
	 * - The pivot element used for the partitioning step is not used by any
	 *   other block
	 *
	 * If the partitioning step did not fail, a new partition pointer is saved if
	 * neccessary.
	 *
	 * @param pivot The pivot element used for the partition step
	 * @param left Start of the newly created partition (which is the right-most
	 * partition of the block, except for the dead partition).
	 * @return False, iff partition can proceed with the next step. True
	 * otherwise (implying that the last partitioning step has to be repeated
	 * with a new pivot element).
	 */
	bool partition_failed(PivotElement* pivot, size_t left)
	{
		if (left != m_partitions->dead()) {
			/* if rightmost partition contains at least 1 item, add a partition
			   pointer */
			m_partitions->add(left, pivot);
			return false;
		}
		/* all items were partitioned into left or dead partition. Thus, our
		 * rightmost non- empty partition is [old_left, dead[. In other words:
		 * [left, dead[ is an empty partition. Thus, we can release the pivot
		 * element.
		 */
		m_pivots->release(pivot);
		if (pivot->refcnt() == 0) {
			/* The pivot element isn't used anywhere else. Thus, delete it and
			 * tell the caller that partitioning failed (So that he can try again
			 * with a different pivot element).
			 */
			delete pivot;
			return true;
		}
		/* the pivot element that caused [left, dead[ to be empty
		 * is already used by other blocks. We do not need to create the
		 * partition and can continue to partition recursively, but cannot
		 * remove the pivot element or try to partition again with a different
		 * pivot element.
		 */
		return false;
	}

	PivotElement* generate_pivot(size_t left, size_t right, size_t pos)
	{
		std::mt19937 rng;
		size_t seed = std::random_device()();
		//quick and dirty fix to de-randomize tests when in debug mode
#ifdef PHEET_DEBUG_MODE
		seed = 42;
#endif
		rng.seed(seed);
		std::uniform_int_distribution<std::mt19937::result_type> dist_e(left, right);

		Item* item = m_data[m_offset + left];
		size_t upper = item->strategy()->nr_dimensions() - 1;
		std::uniform_int_distribution<std::mt19937::result_type> dist_d(0, upper);

		//TODO: sample
		size_t attempts = 0;
		while (attempts < MAX_ATTEMPTS) {
			//random element from block in the range we need to partition
			item = m_data[m_offset + dist_e(rng)];
			if (item && !item->is_taken_or_dead()) {
				//random dimension
				size_t d = dist_d(rng);

				PivotElement* pivot = new PivotElement(d, item->strategy()->priority_at(d), pos);
				if (m_pivots->try_put(pivot)) {
					return pivot;
				}
			}
			++attempts;
		}
		return nullptr;
	}

private:
	VirtualArray<Item*>& m_data;
	size_t m_offset;
	size_t m_capacity;
	size_t m_size;
	size_t m_lvl;

	PartitionPointers* m_partitions;
	PivotQueue* m_pivots;
	size_t m_failed_attempts;

	ParetoLocalityTaskStorageBlock* m_next = nullptr;
	ParetoLocalityTaskStorageBlock* m_prev = nullptr;

};

} /* namespace pheet */

#endif /* PARETOLOCALITYTASKSTORAGEBLOCK_H_ */
