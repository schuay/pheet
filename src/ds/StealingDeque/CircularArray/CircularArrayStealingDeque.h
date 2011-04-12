/*
 * CircularArrayStealingDeque.h
 *
 *  Created on: 12.04.2011
 *      Author: mwimmer
 */

#ifndef CIRCULARARRAYSTEALINGDEQUE_H_
#define CIRCULARARRAYSTEALINGDEQUE_H_

#include "../../../misc/type_traits.h"

namespace pheet {

template <class CircularArray>
class CircularArrayStealingDeque {
public:
	typedef CircularArray::T T;
	CircularArrayStealingDeque(size_t initial_capacity);
	~CircularArrayStealingDeque();

	void push(T item);
	T pop();
	T peek();
	T steal();

	template <class StealingDeque>
	T steal_append(StealingDeque &other);

	size_t get_length();
	bool is_empty();
	bool is_full();

private:
	size_t top;
	size_t bottom;

	static const size_t top_mask;
	static const size_t top_stamp_mask;
	static const size_t top_stamp_add;

	static const T null_element;

	CircularArray data;
};

// Upper 4th of size_t is reserved for stamp. The rest is for the actual content
template <class CircularArray>
const size_t CircularArrayStealingDeque<CircularArray>::top_mask =
		(numeric_limits<size_t>::max() >> (numeric_limits<size_t>::digits >> 2));

template <class CircularArray>
const size_t CircularArrayStealingDeque<CircularArray>::top_stamp_mask =
		(numeric_limits<size_t>::max() ^ CircularArrayStealingDeque<CircularArray>::top_mask);

template <class CircularArray>
const size_t CircularArrayStealingDeque<CircularArray>::top_stamp_add =
		(((size_t)1) << (numeric_limits<size_t>::digits - (numeric_limits<size_t>::digits >> 2)));

template <class CircularArray>
const T CircularArrayStealingDeque<CircularArray>::null_element = nullable_traits<T>::null_value;

template <class CircularArray>
CircularArrayStealingDeque<CircularArray>::CircularArrayStealingDeque(size_t initial_capacity)
: data(initial_capacity), top(0), bottom(0) {

}

template <class CircularArray>
CircularArrayStealingDeque<CircularArray>::~CircularArrayStealingDeque() {

}

template <class CircularArray>
void CircularArrayStealingDeque<CircularArray>::push(T item) {
	if((bottom - (top & top_mask)) >= (data.get_capacity() - 1))
	{
		data.grow(bottom, top & top_mask);
		MEMORY_FENCE ();

		// after resizing we have to make sure no thread can succeed that read from the old indices
		// We do this by incrementing the stamp of top
		size_t old_top = top;
		int new_top = old_top + top_stamp_add;

		// We don't care if we succeed as long as some thread succeeded to change the stamp
		UINT_CAS(&top, oldTop, newTop);
	}

	data.put(bottom, item);

	// Make sure no thread sees new bottom before data has been put
	MEMORY_FENCE();
	bottom++;
}

template <class CircularArray>
T CircularArrayStealingDeque<CircularArray>::pop() {
	if(bottom == (top & top_stamp_add))
		return NULL;

	bottom--;

	// Make sure the write to bottom is seen by all threads
	// requires a write barrier
	MEMORY_FENCE ();

	T ret = data.get(bottom);

	size_t old_top = top;
	size_t masked_top = old_top & top_mask;
	if(bottom > maskedTop)
	{
		return ret;
	}
	if(bottom == maskedTop)
	{
		// Increment stamp (should wrap around)
		size_t new_top = old_top + top_stamp_add;

		if(UINT_CAS(&top, old_top, new_top))
		{
			return ret;
		}
		else {
			// Some other process was faster - the queue is now empty
			bottom++;
		}
	}
	return null_element;
}

template <class CircularArray>
T CircularArrayStealingDeque<CircularArray>::steal() {
	int old_top = top;
	MEMORY_FENCE();

	int masked_top = old_top & top_mask;
	if(bottom <= masked_top) {
		return null_element;
	}

	T ret = data.get(masked_top);

	size_t new_top = old_top + 1 + top_stamp_add;
	if(UINT_CAS(&top, old_top, new_top))
	{
		return ret;
	}

	// if we encounter a race, just return NULL
	// This might even happen on well-filled queues
	return null_element;
}

template <class CircularArray>
template <class StealingDeque>
T CircularArrayStealingDeque<CircularArray>::steal_append(StealingDeque &other) {
	T prev = NULL;
	T curr
}

}

#endif /* CIRCULARARRAYSTEALINGDEQUE_H_ */
