#ifndef BLOCK_H_
#define BLOCK_H_

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

template<class Item, size_t MAX_PARTITION_SIZE>
class Block
{
public:
	typedef typename Item::T T;


	Block(VirtualArray<Item*>* ary, size_t offset, PivotQueue* pivots, size_t lvl = 0)
		: m_data(ary), m_offset(offset), m_size(0), m_lvl(lvl), m_pivots(pivots) {
		assert(ary != nullptr);
		m_capacity = MAX_PARTITION_SIZE * pow(2, m_lvl);
		m_partitions = new PartitionPointers(m_capacity);
	}

	bool try_put(Item* item) {
		if (m_size == m_capacity) {
			return false;
		}
		put(item);
		return true;
	}

	void put(Item* item) {
		assert(m_size < m_capacity);
		//we only put data in a lvl 0 block
		assert(m_lvl == 0);
		put_internal(item);
	}

	/**
	 * Return an item that is not dominated by any other item in this block.
	 *
	 * Do not remove this item from the block. If such an item does not exist,
	 * return nullptr.
	 */
	Item* top() {
		Item* best = nullptr;
		//iterate through items in right-most partition
		for (size_t i = m_partitions->last(); i < m_partitions->end(); i++) {
			//TODO: make readable
			if ((best == nullptr || data_at(i) == nullptr ||
			        !data_at(i)->is_taken_or_dead()
			        && data_at(i)->strategy()->prioritize(*(best->strategy()))) //TODO: ugly
			   ) {
				best = data_at(i);
			}
		}
		//only happens if no more item that is not taken or dead is in current partition
		if (best == nullptr) {
			//fall back to previous partition, if possible
			if (m_partitions->can_fall_back()) {
				m_partitions->fall_back();
				//reduce reference count on pivot element used for that partition step
				m_pivots->release(m_partitions->size());
				//call top() again, now operating on previous partition
				best = top();
			}
		}
		assert(!(best && best->is_taken_or_dead())); //don't return dead or taken items
		return best;
	}

	/**
	 * Take the given item and return its data.
	 *
	 * An item that is taken is marked for deletion/reuse and will not be returned
	 * via a call to top() anymore.
	 */
	T take(Item* item) {
		return item->take();
	}

	Block* merge_next() {
		assert(m_next != nullptr);
		assert(m_next ->lvl() == m_lvl);
		//we only merge full blocks
		assert(m_size == m_capacity);

		//expand this block to cover this as well as next block
		++m_lvl;
		m_capacity <<= 1;
		//merging two full blocks of level lvl results in one full block of lvl+1
		m_size = m_capacity;

		//splice out next
		Block* tmp  = m_next;
		if (tmp->next()) {
			tmp->next()->prev(this);
		}
		m_next = tmp->next();
		delete tmp;

		return this;
	}

	void partition() {
		delete m_partitions;
		m_partitions = new PartitionPointers(m_capacity, m_capacity);
		partition(0, 0, m_capacity - 1);
		drop_dead_items();
	}

	Block* prev() {
		return m_prev;
	}

	void prev(Block* b) {
		m_prev = b;
	}

	Block* next() {
		return m_next;
	}

	void next(Block* b) {
		m_next = b;
	}

	size_t lvl() {
		return m_lvl;
	}

	size_t capacity() {
		return m_capacity;
	}

	size_t offset() {
		return m_offset;
	}

public: //methods required for white box testing
	/**
	 * Allow putting items into blocks of level > 0. Allows for easier testing.
	 */
	void put_internal(Item* item) {
		m_data->push(item);
		m_partitions->end(m_partitions->end() + 1);
		++m_size;
	}


	Item* at(size_t idx) {
		assert(idx < m_capacity);
		return data_at(idx);
	}

	PartitionPointers* partition_pointers() {
		return m_partitions;
	}

private:

	/**
	 * Drop all items in the "dead tasks" partition
	 */
	void drop_dead_items() {
		for (size_t i = m_partitions->dead(); i < m_partitions->end(); i++) {
			Item* item = data_at(i);
			assert(item->is_taken_or_dead());
			if (!item->is_taken()) {
				item->data.drop_item();
			}

			delete item;
			//TODO: make sure item is free'ed or reused
			data_at(i) = nullptr;
		}
	}

	void partition(size_t depth, size_t left, size_t right) {
		assert(left < right);
		assert(depth + 1 == m_partitions->size());
		size_t old_left = left;

		//generate new pivot element if neccesarry
		PivotElement* pivot;
		if (m_pivots->size() <= depth) {
			pivot = generate_pivot(left, right);
			if (pivot == nullptr) {
				/* could not generate suitable pivot element -> Abort partitioning
				 * Right-most partition will exceed MAX_PARTITION_SIZE
				 */
				return;
			}
		} else {
			pivot = m_pivots->get(depth);
		}

		size_t p_dim = pivot->dimension();
		size_t p_val = pivot->value();

		do {
			//TODO: try to call is_taken_or_dead as less as possible (may be expensive, since user implemented)
			while (left < right && data_at(left) && !data_at(left)->is_taken_or_dead()
			        && data_at(left)->strategy()->less_priority(p_dim, p_val)) {
				++left;
			}

			while (left < right && data_at(right) && !data_at(right)->is_taken_or_dead()
			        && (data_at(right)->strategy()->greater_priority(p_dim, p_val)
			            || data_at(right)->strategy()->equal_priority(p_dim, p_val))) {
				--right;
			}

			if (left != right) {
				if (!data_at(right) || data_at(right)->is_taken_or_dead()) {
					//right is dead
					if (m_partitions->dead() - right == 1) {
						//element after right is dead too. Advance dead and right.
						//This is safe since left < right
						m_partitions->dead(right);
						right--;
						assert(left <= right);
					} else {
						//swap right with rightmost non-dead element
						m_partitions->decrease_dead();
						swap(right, m_partitions->dead());
					}
				} else if (!data_at(left) || data_at(left)->is_taken_or_dead()) {
					/* left is dead. Note that left+1==dead may never occur while
					 * left < right, since right < dead holds. */
					assert(left + 1 < m_partitions->dead());
					/* swap left with rightmost non-dead element. This may swap
					 * an element >=pivot to left, but we will not advance left.
					 * Progress is made by putting one dead element into it'S final
					 * place */
					m_partitions->decrease_dead();
					swap(left, m_partitions->dead());
					//if now right == dead, advance right
					if (m_partitions->dead() == right) {
						right--;
					}
				} else {
					/* neither left nor right are dead. Swap left and right */
					swap(left, right);
				}
			}
		} while (left < right);

		assert(left == right);

		//check if left points to dead item
		if (!data_at(left) || data_at(left)->is_taken_or_dead()) {
			//assert(left + 1 == m_partitions->dead());
			m_partitions->decrease_dead();
		}

		//check if item at left belongs to left or right partition
		if (data_at(left)->strategy()->greater_priority(p_dim, p_val)) {
			++left;
		}

		assert(left <= m_partitions->dead());

		if (left != m_partitions->dead()) {
			/* if rightmost partition contains at least 1 item, add a partition
			   pointer */
			add_partition_pointer(left);
			m_failed_attempts = 0;
		} else {
			/* all items were partitioned into left or dead partition. Thus, our
			 * rightmost partition is [old_left, dead[. In other words: [left, dead[
			 * is an empty partition. If the pivot element used to generate this
			 * (empty) partition is not used in other blocks, release it and try
			 * again. Else, add a partition pointer and continue.
			 */
			size_t pivot_idx = m_pivots->size() - 1;
			if (m_pivots->refcnt(pivot_idx) == 1) {
				m_pivots->release(pivot_idx);
				left = old_left;
				/* If right-most partition > MAX_PARTITION_SIZE, we will partition again.
				 * This could potentially run indefinitely (think of all items having
				 * the same priority vector). Thus, bound the number of subsequent
				 * partitioning attempts that do not create a new partition.
				 */
				++m_failed_attempts;
			} else {
				/* the pivot element that caused [left, dead[ to be empty
				 * is already in the PivotQueue. Create the partition pointer for the
				 * empty partition and continue partitioning
				 */
				/* TODO: do we really need to store the partition pointer? maybe
				 * we can just skip it and continue partitioning with the next
				 * pivot element?
				 */
				add_partition_pointer(left);
				m_failed_attempts = 0;
			}
		}

		if ((m_partitions->dead() - left > MAX_PARTITION_SIZE) && m_failed_attempts < MAX_ATTEMPTS) {
			partition(depth + 1, left, m_partitions->dead() - 1);
		}
		m_failed_attempts = 0;
	}

	void swap(size_t left, size_t right) {
		assert(left < right);
		m_data->swap(left + m_offset, right + m_offset);
	}

	//TODO: use VirtualArray::get and ::set instead
	Item*& data_at(size_t idx) {
		assert(idx < m_capacity);
		return (*m_data)[m_offset + idx];
	}

	//TODO: move to class PivotQueue? Problem: need range of current elements
	PivotElement* generate_pivot(size_t left, size_t right) {
		std::mt19937 rng;
		rng.seed(std::random_device()());

		std::uniform_int_distribution<std::mt19937::result_type> dist_e(left, right);

		size_t upper = data_at(left)->strategy()->nr_dimensions() - 1;
		std::uniform_int_distribution<std::mt19937::result_type> dist_d(0, upper);

		//TODO: sample
		size_t attempts = 0;
		while (attempts < MAX_ATTEMPTS) {
			//random element from block in the range we need to partition
			Item* item = data_at(dist_e(rng));
			//random dimension
			size_t d = dist_d(rng);

			PivotElement* pivot = new PivotElement(d, item->strategy()->priority_at(d));
			if (m_pivots->try_put(pivot)) {
				return pivot;
			}
			++attempts;
		}
		return nullptr;
	}

private:

	void add_partition_pointer(size_t idx) {
		m_partitions->add(idx);
		m_partitions->last(idx);
	}

private:
	VirtualArray<Item*>* m_data;
	size_t m_offset;
	size_t m_capacity;
	size_t m_size;
	size_t m_lvl;

	PartitionPointers* m_partitions;
	PivotQueue* m_pivots;
	size_t m_failed_attempts;

	Block* m_next = nullptr;
	Block* m_prev = nullptr;

};

#endif /* BLOCK_H_ */
