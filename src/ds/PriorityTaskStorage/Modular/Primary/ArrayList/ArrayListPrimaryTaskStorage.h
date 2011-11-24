/*
 * ArrayListPrimaryTaskStorage.h
 *
 *  Created on: Nov 24, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LINKEDARRAYLISTPRIMARYTASKSTORAGE_H_
#define LINKEDARRAYLISTPRIMARYTASKSTORAGE_H_

#include "../../../../../settings.h"
#include "../../../../../sched/strategies/BaseStrategy.h"
#include "../../../../../primitives/Backoff/Exponential/ExponentialBackoff.h"

#include "ArrayListPrimaryTaskStoragePerformanceCounters.h"
#include "ArrayListPrimaryTaskStorageIterator.h"
#include "ArrayListPrimaryTaskStorageControlBlock.h"

namespace pheet {

template <typename TT>
struct ArrayListPrimaryTaskStorageItem {
	TT data;
	BaseStrategy* s;
	prio_t pop_prio;
	size_t index;
};

// BLOCK_SIZE has to be a power of 2 to work with wrap-around of index
template <typename TT, size_t BLOCK_SIZE = 64>
class ArrayListPrimaryTaskStorage {
public:
	typedef TT T;
	typedef ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE> ThisType;
	// Not completely a standard iterator, as it doesn't support a dereference operation, but this makes implementation simpler for now (and even more lightweight)
	typedef ArrayListPrimaryTaskStorageIterator<ThisType> iterator;
	typedef ArrayListPrimaryTaskStoragePerformanceCounters PerformanceCounters;
	typedef ArrayListPrimaryTaskControlBlock<ThisType> ControlBlock;
	typedef ArrayListPrimaryTaskStorageItem<T> Item;
	typedef ExponentialBackoff Backoff;

	ArrayListPrimaryTaskStorage(size_t expected_capacity);
//	ArrayListPrimaryTaskStorage(size_t expected_capacity, PerformanceCounters& perf_count);
	~ArrayListPrimaryTaskStorage();

	iterator begin(PerformanceCounters& pc);
	iterator end(PerformanceCounters& pc);

	T take(iterator item, PerformanceCounters& pc);
	bool is_taken(iterator item, PerformanceCounters& pc);
	prio_t get_steal_priority(iterator item, PerformanceCounters& pc);
	size_t get_task_id(iterator item, PerformanceCounters& pc);

	template <class Strategy>
	void push(Strategy& s, T item, PerformanceCounters& pc);
	T pop(PerformanceCounters& pc);
	T peek(PerformanceCounters& pc);

	size_t get_length(PerformanceCounters& pc);
	bool is_empty(PerformanceCounters& pc);
	bool is_full(PerformanceCounters& pc);

	// Can be called by the scheduler every time it is idle to perform some routine maintenance
	void perform_maintenance(PerformanceCounters& pc);

	static void print_name();

	static size_t const block_size;
private:
	T local_take(iterator item, PerformanceCounters& pc);
	void clean(PerformanceCounters& pc);

	ControlBlock* acquire_control_block();
	void create_next_control_block(PerformanceCounters& pc);

	enum {num_control_blocks = 64};
	ControlBlock control_blocks[num_control_blocks];

	size_t start_index;
	size_t end_index;
	// Updated on pushes and pops, but doesn't reflect steals, therefore this number is an estimate.
	// If estimated length of current control block is smaller, use this number instead
	// When clean() is called and queue is already empty, length is guaranteed to be 0
	size_t length;

	size_t current_control_block_id;
	size_t cleanup_control_block_id;
	ControlBlock* current_control_block;
	size_t current_control_block_item_index;

//	PerformanceCounters perf_count;

	static const T null_element;

	friend class iterator;
};

template <typename TT, size_t BLOCK_SIZE>
const TT ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::null_element = nullable_traits<T>::null_value;

template <typename TT, size_t BLOCK_SIZE>
const size_t ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::block_size = BLOCK_SIZE;

template <typename TT, size_t BLOCK_SIZE>
inline ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::ArrayListPrimaryTaskStorage(size_t expected_capacity)
: start_index(0), end_index(0), current_control_block_id(0), cleanup_control_block_id(0), current_control_block(control_blocks), current_control_block_item_index(0) {
	current_control_block->init_empty(0);
}
/*
template <typename TT, size_t BLOCK_SIZE>
inline ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::ArrayListPrimaryTaskStorage(size_t expected_capacity, PerformanceCounters& perf_count)
: top(0), bottom(0), data(expected_capacity), perf_count(perf_count) {

}*/

template <typename TT, size_t BLOCK_SIZE>
inline ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::~ArrayListPrimaryTaskStorage() {

}

template <typename TT, size_t BLOCK_SIZE>
typename ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::iterator ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::begin(PerformanceCounters& pc) {
	return iterator(start_index);
}

template <typename TT, size_t BLOCK_SIZE>
typename ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::iterator ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::end(PerformanceCounters& pc) {
	return iterator(end_index);
}

template <typename TT, size_t BLOCK_SIZE>
TT ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::local_take(iterator item, PerformanceCounters& pc) {
	assert(item < bottom);

	ArrayListPrimaryTaskStorageItem<T>& ptsi = data.get(item);

	if(ptsi.index != item) {
		pc.num_unsuccessful_takes.incr();
		return null_element;
	}
	if(!SIZET_CAS(&(ptsi.index), item, item + 1)) {
		pc.num_unsuccessful_takes.incr();
		return null_element;
	}
	delete ptsi.s;

	pc.num_successful_takes.incr();
	return ptsi.data;
}

template <typename TT, size_t BLOCK_SIZE>
TT ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::take(iterator item, PerformanceCounters& pc) {
	assert(item != end());

	ArrayListPrimaryTaskStorageItem<T>* ptsi = item.dereference(this);

	if(ptsi == NULL || ptsi->index != item) {
		pc.num_unsuccessful_takes.incr();
		return null_element;
	}
	if(!SIZET_CAS(&(ptsi->index), item, item + 1)) {
		pc.num_unsuccessful_takes.incr();
		return null_element;
	}

	pc.num_successful_takes.incr();
	return ptsi->data;
}

template <typename TT, size_t BLOCK_SIZE>
bool ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::is_taken(iterator item, PerformanceCounters& pc) {
	return item.dereference(this)->index != item;
}

template <typename TT, size_t BLOCK_SIZE>
prio_t ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::get_steal_priority(iterator item, PerformanceCounters& pc) {
	return item.dereference(this)->s->get_steal_priority();
}

/*
 * Returns a task id specifying the insertion order. some strategies use the insertion order
 * to prioritize tasks.
 * The task_id is local per queue. When a task is stolen it is assigned a new task id
 * (therefore automatically rebasing the strategy)
 * Warning! If multiple tasks are stolen and inserted into another queue, they have to be
 * inserted in the previous insertion order! (by increasing task_id)
 */
template <typename TT, size_t BLOCK_SIZE>
size_t ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::get_task_id(iterator item, PerformanceCounters& pc) {
	return item.get_index();
}

template <typename TT, size_t BLOCK_SIZE>
template <class Strategy>
inline void ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::push(Strategy& s, T item, PerformanceCounters& pc) {
	pc.push_time.start_timer();

	size_t offset = current_control_block->get_data()[current_control_block_item_index].offset;
	if(end_index - offset == block_size) {
		++current_control_block_item_index;
		if(current_control_block_item_index == current_control_block->get_length()) {
			create_next_control_block();
		}
		offset = current_control_block->get_data()[current_control_block_item_index].offset;
	}
	size_t item_index = end_index - offset;
	assert(item_index < block_size);

	ArrayListPrimaryTaskStorageItem<TT> to_put;
	to_put.data = item;
	to_put.s = new Strategy(s);
	to_put.pop_prio = s.get_pop_priority(bottom);
	to_put.index = end_index;

	current_control_block->get_data()[current_control_block_item_index].data[item_index] = to_put;

	MEMORY_FENCE();

	++length;
	++end_index;
	pc.push_time.stop_timer();
}

template <typename TT, size_t BLOCK_SIZE>
inline TT ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::pop(PerformanceCounters& pc) {
	pc.total_size_pop.add(bottom - top);
	pc.pop_time.start_timer();
	T ret;
	do {
		iterator i = begin(pc);
		while(i != end(pc) && is_taken(i, pc)) {
			++i;
		}

		if(i == end(pc)) {
			pc.num_unsuccessful_pops.incr();
			pc.pop_time.stop_timer();
			return null_element;
		}
		top = i;

		iterator best = i;
		prio_t best_prio = data.get(i).pop_prio;
		++i;
		for(; i != end(pc); ++i) {
			if(!is_taken(i, pc)) {
				prio_t tmp_prio = data.get(i).pop_prio;
				if(tmp_prio > best_prio) {
					best = i;
					best_prio = tmp_prio;
				}
			}
		}
		ret = local_take(best, pc);
	} while(ret == null_element);

	--length;
	pc.num_successful_pops.incr();
	pc.pop_time.stop_timer();
	return ret;
}

template <typename TT, size_t BLOCK_SIZE>
inline TT ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::peek(PerformanceCounters& pc) {
	iterator i = begin(pc);
	while(i != end(pc) && is_taken(top, pc)) {
		++i;
	}

	if(i == end(pc)) {
		return null_element;
	}
	top = i;

	iterator best = i;
	ArrayListPrimaryTaskStorageItem<T> ret_item = data.get(i);
	prio_t best_prio = ret_item.pop_prio;
	++i;
	for(; i != end(pc); ++i) {
		if(!is_taken(i, pc)) {
			ArrayListPrimaryTaskStorageItem<T> tmp_item = data.get(i);
			prio_t tmp_prio = tmp_item.pop_prio;
			if(tmp_prio > best_prio) {
				best = i;
				best_prio = tmp_prio;
				ret_item = tmp_item;
			}
		}
	}

	return ret_item.data;
}

template <typename TT, size_t BLOCK_SIZE>
inline size_t ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::get_length(PerformanceCounters& pc) {
	return length;
}

template <typename TT, size_t BLOCK_SIZE>
inline bool ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::is_empty(PerformanceCounters& pc) {
	clean(pc);
	return length == 0;
}

template <typename TT, size_t BLOCK_SIZE>
inline bool ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::is_full(PerformanceCounters& pc) {
	return false;
}

template <typename TT, size_t BLOCK_SIZE>
void ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::clean(PerformanceCounters& pc) {
	while(cleanup_control_block_id != current_control_block_id) {
		if(!control_blocks[cleanup_control_block_id].try_cleanup()) {
			break;
		}
	}

	size_t upper_bound_length = 0;
	size_t l = current_control_block->get_length();
	ControlBlock::Item* ccb_data = current_control_block->get_data();
	for(size_t i = 0; i < l; ++i) {
		size_t end_pos = end_index - current_control_block->offset;
		if(end_pos < block_size) {
			current_control_block->clean_item_until_limit(i, end_index);
			upper_bound_length += end_index - ccb_data[i].first;
		}
		else {
			current_control_block->clean_item(i);
			upper_bound_length += (ccb_data[i].offset + Storage::block_size) - ccb_data[i].first;
		}
	}
	if(upper_bound_length < length) {
		length = upper_bound_length;
	}
}

template <typename TT, size_t BLOCK_SIZE>
void ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::print_name() {
	std::cout << "ArrayListPrimaryTaskStorage";
}

template <typename TT, size_t BLOCK_SIZE>
ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::ControlBlock* ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::acquire_control_block() {
	Backoff bo;
	while(true) {
		ControlBlock* cb = current_control_block;
		size_t ni = cb->num_iterators;
		if(ni != std::numeric_limits<size_t>::max()) {
			// Control block is active. Try acquiring it
			if(cb->register_iterator()) {
				return cb;
			}
		}
		bo.backoff();
	}
}

template <typename TT, size_t BLOCK_SIZE>
void ArrayListPrimaryTaskStorage<TT, BLOCK_SIZE>::create_next_control_block(PerformanceCounters& pc) {
	clean(pc);

	size_t new_id = (current_control_block_id + 1) % num_control_blocks;
	assert(new_id != cleanup_control_block_id);
	size_t upper_bound_length = control_blocks[new_id].configure_as_successor(current_control_block);
	if(upper_bound_length < length) {
		length = upper_bound_length;
	}

	MEMORY_FENCE();

	current_control_block_id = new_id;
	current_control_block = control_blocks + new_id;
}

}

#endif /* LINKEDARRAYLISTPRIMARYTASKSTORAGE_H_ */
