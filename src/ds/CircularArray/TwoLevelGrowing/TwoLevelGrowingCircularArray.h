/*
 * TwoLevelGrowingCircularArray.h
 *
 *  Created on: 12.04.2011
 *      Author: mwimmer
 */

#ifndef TWOLEVELGROWINGCIRCULARARRAY_H_
#define TWOLEVELGROWINGCIRCULARARRAY_H_

#include "../../../../settings.h"
#include <assert.h>

#include "../../../misc/bitops.h"

namespace pheet {

template <typename T, size_t MAX_BUCKETS>
class TwoLevelGrowingCircularArray {
public:
	typedef T T;

	TwoLevelGrowingCircularArray(size_t initial_capacity);
	~TwoLevelGrowingCircularArray();

	size_t get_capacity();
	bool is_growable();

	T get(size_t i);
	void put(size_t i, T value);

	void grow(size_t bottom, size_t top);

private:
	const size_t initial_buckets;
	size_t buckets;
	size_t capacity;
	T* data[MAX_BUCKETS];
};

template <typename T, size_t MAX_BUCKETS>
TwoLevelGrowingCircularArray<T, MAX_BUCKETS>::TwoLevelGrowingCircularArray(size_t initial_capacity)
: initial_buckets(find_last_bit_set(initial_capacity - 1) + 1), buckets(initial_buckets), capacity(1 << (buckets - 1)) {
	assert(buckets <= MAX_BUCKETS);

	T* ptr = new T[capacity];
	data[0] = ptr;
	ptr++;
	for(size_t i = 1; i < buckets; i++)
	{
		data[i] = ptr;
		ptr += 1 << (i - 1);
	}
}

template <typename T, size_t MAX_BUCKETS>
TwoLevelGrowingCircularArray<T, MAX_BUCKETS>::~TwoLevelGrowingCircularArray() {
	delete[] (data[0]);
	for(size_t i = initial_buckets; i < buckets; i++)
		delete [](data[i]);
}

template <typename T, size_t MAX_BUCKETS>
size_t TwoLevelGrowingCircularArray<T, MAX_BUCKETS>::get_capacity() {
	return capacity;
}

template <typename T, size_t MAX_BUCKETS>
bool TwoLevelGrowingCircularArray<T, MAX_BUCKETS>::is_growable() {
	return buckets < MAX_BUCKETS;
}

template <typename T, size_t MAX_BUCKETS>
T TwoLevelGrowingCircularArray<T, MAX_BUCKETS>::get(size_t i) {
	i = i % capacity;
	size_t hb = find_last_bit_set(i);
	return data[hb][i ^ ((1 << (hb)) >> 1)];
}

template <typename T, size_t MAX_BUCKETS>
void TwoLevelGrowingCircularArray<T, MAX_BUCKETS>::put(size_t i, T value) {
	i = i % capacity;
	size_t hb = find_last_bit_set(i);
	data[hb][i ^ ((1 << (hb)) >> 1)] = value;
}

template <typename T, size_t MAX_BUCKETS>
void TwoLevelGrowingCircularArray<T, MAX_BUCKETS>::grow(size_t bottom, size_t top) {
	assert(is_growable());

	data[buckets] = new T[capacity];
	buckets++;
	size_t newCapacity = capacity << 1;

	size_t start = top;
	size_t startMod = top % capacity;
	if(startMod == (top % newCapacity))
	{	// Make sure we don't iterate through useless indices
		start += capacity - startMod;
	}
	for(size_t i = start; i < bottom; i++) {
		size_t oldI = i % capacity;
		size_t newI = i % newCapacity;
		if(oldI != newI)
		{
			size_t oldBit = find_last_bit_set(oldI);
			size_t newBit = find_last_bit_set(newI);
			data[newBit][newI ^ ((1 << (newBit)) >> 1)] =
				data[oldBit][oldI ^ ((1 << (oldBit)) >> 1)];
		}
		else
		{	// Make sure we don't iterate through useless indices
			break;
		}
	}

	// Do we really need this? I guess we have to ensure that threads don't have
	// some senseless pointers in their data array
	MEMORY_FENCE ();
	capacity = newCapacity;
}

}

#endif /* TWOLEVELGROWINGCIRCULARARRAY_H_ */
