/*
 * CircularArrayStealingDeque.h
 *
 *  Created on: 12.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef CIRCULARARRAYSTEALINGDEQUE_H_
#define CIRCULARARRAYSTEALINGDEQUE_H_

#include "../../../misc/type_traits.h"
#include "../../../misc/atomics.h"

#include <limits>
#include <iostream>

namespace pheet {

template <class Scheduler, typename TT, template <typename S> class CircularArray>
class CircularArrayStealingDeque {
public:
	typedef TT T;
	CircularArrayStealingDeque(size_t initial_capacity);
	CircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<Scheduler, stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<Scheduler, stealing_deque_count_pop_cas>& num_pop_cas);
	~CircularArrayStealingDeque();

	void push(T item);
	T pop();
	T peek();
	T steal();

	T steal_push(CircularArrayStealingDeque<Scheduler, TT, CircularArray> &other);

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

	CircularArray<T> data;

	BasicPerformanceCounter<Scheduler, stealing_deque_count_steals> num_stolen;
	BasicPerformanceCounter<Scheduler, stealing_deque_count_steals> num_pop_cas;
};

// Upper 4th of size_t is reserved for stamp. The rest is for the actual content
template <class Scheduler, typename TT, template <typename S> class CircularArray>
const size_t CircularArrayStealingDeque<Scheduler, TT, CircularArray>::top_mask =
		(std::numeric_limits<size_t>::max() >> (std::numeric_limits<size_t>::digits >> 2));

template <class Scheduler, typename TT, template <typename S> class CircularArray>
const size_t CircularArrayStealingDeque<Scheduler, TT, CircularArray>::top_stamp_mask =
		(std::numeric_limits<size_t>::max() ^ CircularArrayStealingDeque<Scheduler, TT, CircularArray>::top_mask);

template <class Scheduler, typename TT, template <typename S> class CircularArray>
const size_t CircularArrayStealingDeque<Scheduler, TT, CircularArray>::top_stamp_add =
		(((size_t)1) << (std::numeric_limits<size_t>::digits - (std::numeric_limits<size_t>::digits >> 2)));

template <class Scheduler, typename TT, template <typename S> class CircularArray>
const TT CircularArrayStealingDeque<Scheduler, TT, CircularArray>::null_element = nullable_traits<T>::null_value;

template <class Scheduler, typename TT, template <typename S> class CircularArray>
CircularArrayStealingDeque<Scheduler, TT, CircularArray>::CircularArrayStealingDeque(size_t initial_capacity)
: top(0), bottom(0), data(initial_capacity) {

}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
CircularArrayStealingDeque<Scheduler, TT, CircularArray>::CircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<Scheduler, stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<Scheduler, stealing_deque_count_pop_cas>& num_pop_cas)
: top(0), bottom(0), data(initial_capacity), num_stolen(num_stolen), num_pop_cas(num_pop_cas) {

}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
CircularArrayStealingDeque<Scheduler, TT, CircularArray>::~CircularArrayStealingDeque() {

}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
void CircularArrayStealingDeque<Scheduler, TT, CircularArray>::push(T item) {
	assert(bottom >= (top & top_mask));
	if((bottom - (top & top_mask)) >= (data.get_capacity()))
	{
		data.grow(bottom, top & top_mask);
		MEMORY_FENCE ();

		// after resizing we have to make sure no thread can succeed that read from the old indices
		// We do this by incrementing the stamp of top
		size_t old_top = top;
		size_t new_top = old_top + top_stamp_add;

		// We don't care if we succeed as long as some thread succeeded to change the stamp
		SIZET_CAS(&top, old_top, new_top);
	}

	data.put(bottom, item);

	// Make sure no thread sees new bottom before data has been put
	MEMORY_FENCE();
	bottom++;
}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
TT CircularArrayStealingDeque<Scheduler, TT, CircularArray>::pop() {
	if(bottom == (top & top_mask))
		return null_element;

	bottom--;

	// Make sure the write to bottom is seen by all threads
	// requires a write barrier
	MEMORY_FENCE ();

	T ret = data.get(bottom);

	size_t old_top = top;
	size_t masked_top = old_top & top_mask;
	if(bottom > masked_top)
	{
		return ret;
	}
	if(bottom == masked_top)
	{
		// Increment stamp (should wrap around)
		size_t new_top = old_top + top_stamp_add;

		num_pop_cas.incr();
		if(SIZET_CAS(&top, old_top, new_top))
		{
		//	std::cout << "pop " << bottom << std::endl;
			return ret;
		}
		else {
			// Some other process was faster - the queue is now empty
			bottom++;
		}
	}
	else {
		assert(bottom == masked_top - 1);
		bottom = masked_top;
	}
	return null_element;
}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
TT CircularArrayStealingDeque<Scheduler, TT, CircularArray>::peek() {
	if(bottom == (top & top_mask))
		return null_element;

	return data.get(bottom);
}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
TT CircularArrayStealingDeque<Scheduler, TT, CircularArray>::steal() {
	size_t old_top = top;
	MEMORY_FENCE();

	size_t masked_top = old_top & top_mask;
	if(bottom <= masked_top) {
		return null_element;
	}

	T ret = data.get(masked_top);

	size_t new_top = old_top + 1 + top_stamp_add;
	if(UINT_CAS(&top, old_top, new_top))
	{
		num_stolen.incr();
		return ret;
	}

	// if we encounter a race, just return NULL
	// This might even happen on well-filled queues
	return null_element;
}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
TT CircularArrayStealingDeque<Scheduler, TT, CircularArray>::steal_push(CircularArrayStealingDeque<Scheduler, TT, CircularArray> &other) {
	T prev = null_element;
	T curr = null_element;
	size_t max_steal = get_length() / 3;

	for(size_t i = 0; i < max_steal; i++) {
		curr = steal();
		if(curr == null_element) {
			return prev;
		}
		else if(prev != null_element) {
			num_stolen.incr();
			other.push(prev);
		}
		else {
			num_stolen.incr();
		}
		prev = curr;
	}
	return prev;
}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
size_t CircularArrayStealingDeque<Scheduler, TT, CircularArray>::get_length() {
	return (bottom - (top & top_mask));
}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
bool CircularArrayStealingDeque<Scheduler, TT, CircularArray>::is_empty() {
	return get_length() == 0;
}

template <class Scheduler, typename TT, template <typename S> class CircularArray>
bool CircularArrayStealingDeque<Scheduler, TT, CircularArray>::is_full() {
	return (!data.is_growable()) && (get_length() >= data.get_capacity());
}

}

#endif /* CIRCULARARRAYSTEALINGDEQUE_H_ */
