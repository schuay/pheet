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
	typename Storage::Item* data;
};

template <class Storage>
class ArrayListPrimaryTaskStorageControlBlock {
public:
	typedef ArrayListPrimaryTaskStorageControlBlock<Storage> ThisType;
	typedef ArrayListPrimaryTaskStorageControlBlockItem<Storage> Item;

	ArrayListPrimaryTaskStorageControlBlock();
	~ArrayListPrimaryTaskStorageControlBlock();

	bool try_register_iterator();
	void deregister_iterator();

	Item* get_data();
	size_t get_length();
	size_t get_new_item_index();

	size_t configure_as_successor(ThisType* prev);
	void init_empty(size_t offset);

	bool try_cleanup();
	void clean_item(size_t item);
	void clean_item_until(size_t item, size_t limit);
	void finalize();
	void finalize_item(size_t item);
	void finalize_item_until(size_t item, size_t limit);
private:

	size_t num_iterators;
	size_t num_passed_iterators;

	Item* data;
	size_t length;
	size_t new_item_index;
};

template <class Storage>
ArrayListPrimaryTaskStorageControlBlock<Storage>::ArrayListPrimaryTaskStorageControlBlock()
: num_iterators(std::numeric_limits<size_t>::max()), num_passed_iterators(0) {

}

template <class Storage>
ArrayListPrimaryTaskStorageControlBlock<Storage>::~ArrayListPrimaryTaskStorageControlBlock() {

}

template <class Storage>
inline bool ArrayListPrimaryTaskStorageControlBlock<Storage>::try_register_iterator() {
	size_t ni = num_iterators;
	if(ni != std::numeric_limits<size_t>::max()) {
		return SIZET_CAS(&(num_iterators), ni, ni + 1);
	}
	return false;
}

template <class Storage>
inline void ArrayListPrimaryTaskStorageControlBlock<Storage>::deregister_iterator() {
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
inline size_t ArrayListPrimaryTaskStorageControlBlock<Storage>::get_new_item_index() {
	return new_item_index;
}

template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::clean_item(size_t item) {
	size_t limit = data[item].offset + Storage::block_size;
	clean_item_until(item, limit);
}

template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::clean_item_until(size_t item, size_t limit) {
	size_t offset = data[item].offset;
	while(data[item].first != limit && data[item].data[data[item].first - offset].index != data[item].first) {
		assert(data[item].data[data[item].first - offset].index == data[item].first + 1);
		++(data[item].first);
	}
}
template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::finalize() {
	assert(num_iterators == num_passed_iterators);
	delete[] data;
}

template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::finalize_item(size_t item) {
	assert(num_iterators == num_passed_iterators);
	size_t limit = data[item].offset + Storage::block_size;
	finalize_item_until(item, limit);
}

template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::finalize_item_until(size_t item, size_t limit) {
	assert(num_iterators == num_passed_iterators);
	size_t offset = data[item].offset;
	for(size_t i = offset; i != limit; ++i) {
		assert(data[item].data[i - offset].index == i + 1);
		delete data[item].data[i - offset].s;
	}
	delete[] data[item].data;
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

		new_item_index = num_active;
		size_t last_offset = prev->data[prev->length - 1].offset;
		for(size_t i = num_active; i < length; ++i) {
			last_offset += Storage::block_size;
			data[i].offset = last_offset;
			data[i].first = last_offset;
			data[i].data = new typename Storage::Item[Storage::block_size];
		}
	}

	// We don't need a fence before this, as the pointer to this block is only made available after a fence
	num_iterators = 0;

	// Returns an estimate of the number of remaining elements
	return remaining_elements;
}

template <class Storage>
void ArrayListPrimaryTaskStorageControlBlock<Storage>::init_empty(size_t offset) {
	assert(num_iterators == std::numeric_limits<size_t>::max());
	assert(num_passed_iterators == 0);

	// We don't need a fence before this, as the pointer to this block is only made available after a fence
	num_iterators = 0;
	new_item_index = 0;

	data = new Item[1];
	length = 1;

	data[0].offset = offset;
	data[0].first = offset;
	data[0].data = new typename Storage::Item[Storage::block_size];
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
					delete[] data[i].data;
				}
			}
			delete[] data;
			num_passed_iterators = 0;
			return true;
		}
	}
	return false;
}

}

#endif /* LINKEDARRAYLISTPRIMARYTASKSTORAGECONTROLBLOCK_H_ */
