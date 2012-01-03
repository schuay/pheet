/*
 * Heap.h
 *
 *  Created on: Nov 30, 2011
 *      Author: mwimmer
 *	   License: Pheet license
 */

#ifndef HEAP_H_
#define HEAP_H_

#include "../../../settings.h"

#include <string.h>
#include <iostream>

namespace pheet {

/*
 * MaxHeap
 */
template <typename TT, class Comparator = std::less<TT> >
class Heap {
public:
	typedef TT T;

	Heap();
	Heap(Comparator const& comp);
	~Heap();

	void push(T item);
	T peek();
	T pop();

	size_t get_length();
	bool is_empty();

	static void print_name();

private:
	void bubble_up(size_t index);
	void bubble_down(size_t index);

	size_t capacity;
	size_t length;
	TT* data;
	Comparator is_less;
};

template <typename TT, class Comparator>
Heap<TT, Comparator>::Heap()
: capacity(4), length(0), data(new TT[capacity]) {

}

template <typename TT, class Comparator>
Heap<TT, Comparator>::Heap(Comparator const& comp)
: capacity(4), length(0), data(new TT[capacity]), is_less(comp) {

}

template <typename TT, class Comparator>
Heap<TT, Comparator>::~Heap() {
	delete[] data;
}

template <typename TT, class Comparator>
void Heap<TT, Comparator>::push(T item) {
	if(length == capacity) {
		size_t new_capacity = capacity << 1;
		T* new_data = new T[new_capacity];
		memcpy(new_data, data, sizeof(T) * capacity);
		delete[] data;
		data = new_data;
		capacity = new_capacity;
	}

	data[length] = item;
	bubble_up(length);
	++length;
}

template <typename TT, class Comparator>
TT Heap<TT, Comparator>::peek() {
	return data[0];
}


template <typename TT, class Comparator>
TT Heap<TT, Comparator>::pop() {
	T ret = data[0];
	--length;
	data[0] = data[length];
	bubble_down(0);
	return ret;
}

template <typename TT, class Comparator>
size_t Heap<TT, Comparator>::get_length() {
	return length;
}

template <typename TT, class Comparator>
bool Heap<TT, Comparator>::is_empty() {
	return length == 0;
}

template <typename TT, class Comparator>
void Heap<TT, Comparator>::bubble_up(size_t index) {
	size_t next = (index - 1) >> 1;
	while(index > 0 && is_less(data[next], data[index])) {
		std::swap(data[next], data[index]);
		index = next;
		next = (index - 1) >> 1;
	}
}

template <typename TT, class Comparator>
void Heap<TT, Comparator>::bubble_down(size_t index) {
	size_t next = (index << 1) + 1;
	while(next < length) {
		size_t nnext = next + 1;
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
}

template <typename TT, class Comparator>
void Heap<TT, Comparator>::print_name() {
	std::cout << "Heap";
}

}

#endif /* HEAP_H_ */