/*
 * DeHeap.h
 *
 *  Created on: 06.01.2012
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef DEHEAP_H_
#define DEHEAP_H_

#include "../../../settings.h"

#include <string.h>
#include <iostream>

namespace pheet {

/*
 * Double ended heap
 */
template <typename TT, class Comparator = std::less<TT> >
class DeHeap {
public:
	typedef TT T;

	DeHeap();
	DeHeap(Comparator const& comp);
	~DeHeap();

	void push(T item);
	T min();
	T max();
	T pop_min();
	T pop_max();
	T replace_min(T item);
	T replace_max(T item);

	size_t get_length();
	bool is_empty();

	static void print_name();

private:
	void bubble_up_min(size_t index);
	void bubble_up_max(size_t index);
	void bubble_down_min(size_t index);
	void bubble_down_max(size_t index);

	size_t capacity;
	size_t length;
	TT* data;
	Comparator is_less;
};

template <typename TT, class Comparator>
DeHeap<TT, Comparator>::DeHeap()
: capacity(4), length(0), data(new TT[capacity]) {

}

template <typename TT, class Comparator>
DeHeap<TT, Comparator>::DeHeap(Comparator const& comp)
: capacity(4), length(0), data(new TT[capacity]), is_less(comp) {

}

template <typename TT, class Comparator>
DeHeap<TT, Comparator>::~DeHeap() {
	delete[] data;
}

template <typename TT, class Comparator>
void DeHeap<TT, Comparator>::push(T item) {
	if(length == capacity) {
		size_t new_capacity = capacity << 1;
		T* new_data = new T[new_capacity];
		memcpy(new_data, data, sizeof(T) * capacity);
		delete[] data;
		data = new_data;
		capacity = new_capacity;
	}

	data[length] = item;
	if(length & 1) {
		// starts in max heap
		if(is_less(data[length], data[length - 1])) {
			// move to min heap
			std::swap(data[length], data[length - 1]);
			bubble_up_min(length - 1);
		}
		else {
			bubble_up_max(length);
		}
	}
	else if(length > 0) {
		// Starts in min heap
		size_t max_heap_item = ((length - 2) >> 1) | 1;
		if(is_less(data[max_heap_item], data[length])) {
			// move to max heap
			std::swap(data[length], data[max_heap_item]);
			bubble_up_max(max_heap_item);
		}
		else {
			bubble_up_min(length);
		}
	}
	++length;
}

template <typename TT, class Comparator>
TT DeHeap<TT, Comparator>::min() {
	assert(length > 0);
	return data[0];
}

template <typename TT, class Comparator>
TT DeHeap<TT, Comparator>::max() {
	assert(length > 0);
	if(length == 1)
		return data[0];
	return data[1];
}

template <typename TT, class Comparator>
TT DeHeap<TT, Comparator>::pop_min() {
	assert(length > 0);
	T ret = data[0];
	--length;
	data[0] = data[length];
	if(length > 1) {
		bubble_down_min(0);
	}
	return ret;
}

template <typename TT, class Comparator>
TT DeHeap<TT, Comparator>::pop_max() {
	assert(length > 0);
	if(length == 1) {
		length = 0;
		return data[0];
	}
	T ret = data[1];
	--length;
	data[1] = data[length];
	if(length > 1) {
		bubble_down_max(1);
	}
	return ret;
}

template <typename TT, class Comparator>
TT DeHeap<TT, Comparator>::replace_min(T item) {
	assert(length > 0);
	T ret = data[0];
	data[0] = item;
	if(length > 1) {
		bubble_down_min(0);
	}
	return ret;
}

template <typename TT, class Comparator>
TT DeHeap<TT, Comparator>::replace_max(T item) {
	assert(length > 0);
	if(length == 1) {
		T ret = data[0];
		data[0] = item;
		return ret;
	}
	T ret = data[1];
	data[1] = item;
	if(length > 1) {
		bubble_down_max(1);
	}
	return ret;
}

template <typename TT, class Comparator>
size_t DeHeap<TT, Comparator>::get_length() {
	return length;
}

template <typename TT, class Comparator>
bool DeHeap<TT, Comparator>::is_empty() {
	return length == 0;
}

template <typename TT, class Comparator>
void DeHeap<TT, Comparator>::bubble_up_min(size_t index) {
	size_t mask = std::numeric_limits< size_t >::max() ^ 1;
	size_t next = ((index - 2) >> 1) & mask;
	while(index > 0 && is_less(data[index], data[next])) {
		std::swap(data[next], data[index]);
		index = next;
		next = ((index - 1) >> 1) & mask;
	}
}

template <typename TT, class Comparator>
void DeHeap<TT, Comparator>::bubble_up_max(size_t index) {
	size_t next = ((index - 3) >> 1) | 1;
	while(index > 0 && is_less(data[next], data[index])) {
		std::swap(data[next], data[index]);
		index = next;
		next = ((index - 3) >> 1) | 1;
	}
}

template <typename TT, class Comparator>
void DeHeap<TT, Comparator>::bubble_down_min(size_t index) {
	assert((index & 1) == 0);
	size_t next = (index << 1) + 2;
	while(next < length) {
		size_t nnext = next + 2;
		if(nnext < length && is_less(data[nnext], data[next])) {
			next = nnext;
		}
		if(!is_less(data[next], data[index])) {
			break;
		}
		std::swap(data[index], data[next]);
		index = next;
		next = (index << 1) + 2;
	}
	if(next >= length) {
		size_t max_heap_item = index + 1;
		if(max_heap_item >= length) {
			max_heap_item = ((index - 2) >> 1) | 1;
		}
		assert(max_heap_item < length);
		if(is_less(data[max_heap_item], data[index])) {
			swap(data[max_heap_item], data[index]);
			bubble_up_max(max_heap_item);
		}
	}
}

template <typename TT, class Comparator>
void DeHeap<TT, Comparator>::bubble_down_max(size_t index) {
	assert((index & 1) == 1);
	size_t next = (index << 1) + 1;
	while(next < length) {
		size_t nnext = next + 2;
		if(nnext < length && is_less(data[next], data[nnext])) {
			next = nnext;
		}
		if(!is_less(data[index], data[next])) {
			break;
		}
		std::swap(data[index], data[next]);
		index = next;
		next = (index << 1) + 1;
	}
	if(next >= length) {
		size_t min_heap_item = index - 1;
		assert(min_heap_item < length);
		if(is_less(data[index], data[min_heap_item])) {
			swap(data[min_heap_item], data[index]);
			bubble_up_min(min_heap_item);
		}
	}
}

template <typename TT, class Comparator>
void DeHeap<TT, Comparator>::print_name() {
	std::cout << "DeHeap";
}

}


#endif /* DEHEAP_H_ */
