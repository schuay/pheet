/*
 * ArrayListPrimaryTaskStorageControlBlock.h
 *
 *  Created on: Nov 24, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LINKEDARRAYLISTPRIMARYTASKSTORAGECONTROLBLOCK_H_
#define LINKEDARRAYLISTPRIMARYTASKSTORAGECONTROLBLOCK_H_

#include "../../../../../settings.h"

namespace pheet {

template <class Storage>
struct ArrayListPrimaryTaskStorageControlBlockItem {
	// First index in data
	size_t offset;
	// First active index in data (if equals Storage::block_size delete when cleaning up block
	size_t first;
	Storage::Item data[Storage::block_size];
};

template <class Storage>
class ArrayListPrimaryTaskStorageControlBlock {
public:
	typedef ArrayListPrimaryTaskStorageControlBlock<Storage> ThisType;
	typedef ArrayListPrimaryTaskStorageControlBlockItem<Storage> Item;

	ArrayListPrimaryTaskStorageControlBlock();
	~ArrayListPrimaryTaskStorageControlBlock();

	bool register_iterator();
	void deregister_iterator();

	Item* get_data();
	size_t get_length();

	void configure_as_successor(ThisType* prev);
	void init_empty(size_t offset);

	bool try_cleanup();
private:
	void clean_item(size_t item);

	size_t num_iterators;
	size_t num_passed_iterators;

	Item* data;
	size_t length;
};

template <class Storage>
ArrayListPrimaryTaskStorageControlBlock<Storage>::ArrayListPrimaryTaskStorageControlBlock()
: num_iterators(std::numeric_limits<size_t>::max()), num_passed_iterators(0) {

}

template <class Storage>
ArrayListPrimaryTaskStorageControlBlock<Storage>::~ArrayListPrimaryTaskStorageControlBlock() {

}

template <class Storage>
inline bool ArrayListPrimaryTaskStorageControlBlock<Storage>::register_iterator() {
	size_t ni = num_iterators;
	if(ni != std::numeric_limits<size_t>::max()) {
		return SIZET_CAS(&(num_iterators), ni, ni + 1);
	}
	return false;
}

template <class Storage>
inline bool ArrayListPrimaryTaskStorageControlBlock<Storage>::deregister_iterator() {
	SIZET_ATOMIC_ADD(&num_passed_iterators, 1);
}

template <class Storage>
inline ArrayListPrimaryTaskStorageControlBlockItem<Storage>* ArrayListPrimaryTaskStorageControlBlock<Storage>::get_data() {
	return data;
}

template <class Storage>
inline size_t ArrayListPrimaryTaskStorageControlBlock<Storage>::get_length() {
	return length;
}

template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::clean_item(size_t item) {
	size_t limit = data[item].offset + Storage::block_size;
	clean_item_until(item, limit);
}

template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::clean_item_until(size_t item, size_t limit) {
	while(data[item].first != limit && data[item].data[data[item].first].index != data[item].first) {
		assert(data[item].data[data[item].first].index == data[item].first + 1);
		++(data[item].first);
	}
}

template <class Storage>
size_t ArrayListPrimaryTaskStorageControlBlock<Storage>::configure_as_successor(ThisType* prev) {
	assert(num_iterators == std::numeric_limits<size_t>::max());
	assert(num_passed_iterators == 0);

	size_t num_active = 0;
	size_t remaining_elements = 0;
	for(size_t i = 0; i < prev->length; ++i) {
		prev->clean_item(i);
		if(prev->data[i].first != prev->data[i].offset + Storage::block_size) {
			assert(prev->data[i].first < prev->data[i].offset + Storage::block_size);
			remaining_elements += (prev->data[i].offset + Storage::block_size) - prev->data[i].first;
			++num_active;
		}
	}

	if(num_active == 0) {
		init_empty(prev->data[prev->length - 1].offset + Storage::block_size);
	}
	else {
		length = (num_active << 1);
		data = new Item[length];
		for(size_t i = 0, j = 0; i < prev->length; ++i) {
			if(prev->data[i].first != Storage::block_size) {
				data[j] = prev->data[i];
				++j;
			}
		}

		size_t last_offset = prev->data[prev->length - 1].offset;
		for(size_t i = num_active; i < length; ++i) {
			last_offset += Storage::block_size;
			data[i].offset = last_offset;
			data[i].first = last_offset;
		}
	}
	// Returns an estimate of the number of remaining elements
	return remaining_elements;
}

template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::init_empty(size_t offset) {
	assert(num_iterators == std::numeric_limits<size_t>::max());
	assert(num_passed_iterators == 0);

	data = new Item[1];
	length = 1;

	data[0].offset = offset;
	data[0].first = offset;
}

template <class Storage>
bool ArrayListPrimaryTaskStorageControlBlock<Storage>::try_cleanup() {
	size_t ni = num_iterators;
	assert(ni != std::numeric_limits<size_t>::max());
	if(ni == num_passed_iterators) {
		if(SIZET_CAS(&(num_iterators), ni, std::numeric_limits<size_t>::max())) {
			// success! We can now delete everything as there is no iterator in the way and therefore no other threads accessing this ControlBlock
			for(size_t i = 0; i < length; ++i) {
				if(data[i].first == data[i].offset + Storage::block_size) {
					for(size_t j = 0; j < Storage::block_size; ++j) {
						delete data[i].data[j].s;
					}
				}
			}
			delete[] data;
			num_passed_iterators = 0;
		}
	}
	return false;
}

}

#endif /* LINKEDARRAYLISTPRIMARYTASKSTORAGECONTROLBLOCK_H_ */
