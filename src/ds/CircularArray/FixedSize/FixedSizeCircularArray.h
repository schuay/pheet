/*
 * FixedSizeCircularArray.h
 *
 *  Created on: 12.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef FIXEDSIZECIRCULARARRAY_H_
#define FIXEDSIZECIRCULARARRAY_H_

#include "../../../../settings.h"
#include <assert.h>

namespace pheet {

template <typename T>
class FixedSizeCircularArray {
public:
	FixedSizeCircularArray(size_t initial_capacity);
	~FixedSizeCircularArray();

	size_t get_capacity();
	bool is_growable();

	T get(size_t i);
	void put(size_t i, T value);

	void grow(size_t bottom, size_t top);
private:
	size_t const capacity;
	T* data;
};

template <typename T>
FixedSizeCircularArray<T>::FixedSizeCircularArray(size_t initial_capacity)
: capacity(initial_capacity) {
	data = new T[capacity];
}

template <typename T>
FixedSizeCircularArray<T>::~FixedSizeCircularArray() {
	delete[] data;
}

template <typename T>
size_t FixedSizeCircularArray<T>::get_capacity() {
	return capacity;
}

template <typename T>
bool FixedSizeCircularArray<T>::is_growable() {
	return false;
}

template <typename T>
T FixedSizeCircularArray<T>::get(size_t i) {
	return data[i % capacity];
}

template <typename T>
void FixedSizeCircularArray<T>::put(size_t i, T item) {
	data[i % capacity] = item;
}

template <typename T>
void FixedSizeCircularArray<T>::grow(size_t bottom, size_t top) {
	assert(false);
}

}

#endif /* FIXEDSIZECIRCULARARRAY_H_ */
