/*
 * PrimitivePrimaryTaskStorage.h
 *
 *  Created on: 21.09.2011
 *      Author: mwimmer
 */

#ifndef PRIMITIVEPRIMARYTASKSTORAGE_H_
#define PRIMITIVEPRIMARYTASKSTORAGE_H_

#include "../../../../../settings.h"
#include "../../../../../sched/strategies/BaseStrategy.h"

#include "PrimitivePrimaryTaskStoragePerformanceCounters.h"

namespace pheet {

template <typename TT>
struct PrimitivePrimaryTaskStorageItem {
	TT data;
	BaseStrategy* s;
	prio_t pop_prio;
	prio_t steal_prio;
	size_t index;
};

template <typename TT, template <typename S> class CircularArray>
class PrimitivePrimaryTaskStorage {
public:
	typedef TT T;
	// Not completely a standard iterator, as it doesn't support a dereference operation, but this makes implementation simpler for now (and even more lightweight)
	typedef size_t iterator;
	typedef PrimitivePrimaryTaskStoragePerformanceCounters PerformanceCounters;

	PrimitivePrimaryTaskStorage(size_t initial_capacity);
	PrimitivePrimaryTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count);
	~PrimitivePrimaryTaskStorage();

	iterator begin();
	iterator end();

	T take(iterator item);
	bool is_taken(iterator item);
	prio_t get_steal_priority(iterator item);
	size_t get_task_id(iterator item);

	template <class Strategy>
	void push(Strategy& s, T item);
	T pop();
	T peek();

	size_t get_length();
	bool is_empty();
	bool is_full();

	static void print_name();

private:
	void clean();

	size_t top;
	size_t bottom;

	CircularArray<PrimitivePrimaryTaskStorageItem<T> > data;

	PerformanceCounters perf_count;

	static const T null_element;
};

template <typename TT, template <typename S> class CircularArray>
const TT PrimitivePrimaryTaskStorage<TT, CircularArray>::null_element = nullable_traits<T>::null_value;

template <typename TT, template <typename S> class CircularArray>
inline PrimitivePrimaryTaskStorage<TT, CircularArray>::PrimitivePrimaryTaskStorage(size_t initial_capacity)
: top(0), bottom(0), data(initial_capacity) {

}

template <typename TT, template <typename S> class CircularArray>
inline PrimitivePrimaryTaskStorage<TT, CircularArray>::PrimitivePrimaryTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count)
: top(0), bottom(0), data(initial_capacity), perf_count(perf_count) {

}

template <typename TT, template <typename S> class CircularArray>
inline PrimitivePrimaryTaskStorage<TT, CircularArray>::~PrimitivePrimaryTaskStorage() {

}

template <typename TT, template <typename S> class CircularArray>
typename PrimitivePrimaryTaskStorage<TT, CircularArray>::iterator PrimitivePrimaryTaskStorage<TT, CircularArray>::begin() {
	return top;
}

template <typename TT, template <typename S> class CircularArray>
typename PrimitivePrimaryTaskStorage<TT, CircularArray>::iterator PrimitivePrimaryTaskStorage<TT, CircularArray>::end() {
	return bottom;
}

template <typename TT, template <typename S> class CircularArray>
TT PrimitivePrimaryTaskStorage<TT, CircularArray>::take(iterator item) {
	assert(item < bottom);

	PrimitivePrimaryTaskStorageItem<T>& ptsi = data.get(item);

	if(ptsi.index != item) {
		perf_count.num_unsuccessful_takes.incr();
		return null_element;
	}
	T ret = ptsi.data;
	BaseStrategy* s = ptsi.s;
	if(!SIZET_CAS(&(ptsi.index), item, item + 1)) {
		perf_count.num_unsuccessful_takes.incr();
		return null_element;
	}
	delete s;

	perf_count.num_successful_takes.incr();
	return ret;
}

template <typename TT, template <typename S> class CircularArray>
bool PrimitivePrimaryTaskStorage<TT, CircularArray>::is_taken(iterator item) {
	return data.get(item).index != item;
}

template <typename TT, template <typename S> class CircularArray>
prio_t PrimitivePrimaryTaskStorage<TT, CircularArray>::get_steal_priority(iterator item) {
	return data.get(item).steal_prio;
}

/*
 * Returns a task id specifying the insertion order. some strategies use the insertion order
 * to prioritize tasks.
 * The task_id is local per queue. When a task is stolen it is assigned a new task id
 * (therefore automatically rebasing the strategy)
 * Warning! If multiple tasks are stolen and inserted into another queue, they have to be
 * inserted in the previous insertion order! (by increasing task_id)
 */
template <typename TT, template <typename S> class CircularArray>
size_t PrimitivePrimaryTaskStorage<TT, CircularArray>::get_task_id(iterator item) {
	return item;
}

template <typename TT, template <typename S> class CircularArray>
template <class Strategy>
inline void PrimitivePrimaryTaskStorage<TT, CircularArray>::push(Strategy& s, T item) {
	if(bottom - top == data.get_capacity()) {
		clean();
		if(bottom - top == data.get_capacity()) {
			assert(data.is_growable());
			data.grow(bottom, top);
		}
	}

	PrimitivePrimaryTaskStorageItem<TT> to_put;
	to_put.data = item;
	to_put.s = new Strategy(s);
	to_put.pop_prio = s.get_pop_priority(bottom);
	to_put.steal_prio = s.get_steal_priority(bottom);
	to_put.index = bottom;

	data.put(bottom, to_put);

	MEMORY_FENCE();

	bottom++;
}

template <typename TT, template <typename S> class CircularArray>
inline TT PrimitivePrimaryTaskStorage<TT, CircularArray>::pop() {
	perf_count.total_size_pop.add(bottom - top);
	perf_count.pop_time.start_timer();
	T ret;
	do {
		iterator i = begin();
		while(i != end() && is_taken(i)) {
			++i;
		}

		if(i == end()) {
			perf_count.num_unsuccessful_pops.incr();
			perf_count.pop_time.stop_timer();
			return null_element;
		}
		top = i;

		iterator best = i;
		prio_t best_prio = data.get(i).pop_prio;
		++i;
		for(; i != end(); ++i) {
			if(!is_taken(i)) {
				prio_t tmp_prio = data.get(i).pop_prio;
				if(tmp_prio > best_prio) {
					best = i;
					best_prio = tmp_prio;
				}
			}
		}
		ret = take(best);
	} while(ret == null_element);

	perf_count.num_successful_pops.incr();
	perf_count.pop_time.stop_timer();
	return ret;
}

template <typename TT, template <typename S> class CircularArray>
inline TT PrimitivePrimaryTaskStorage<TT, CircularArray>::peek() {
	iterator i = begin();
	while(i != end() && is_taken(top)) {
		++i;
	}

	if(i == end()) {
		return null_element;
	}
	top = i;

	iterator best = i;
	PrimitivePrimaryTaskStorageItem<T> ret_item = data.get(i);
	prio_t best_prio = ret_item.pop_prio;
	++i;
	for(; i != end(); ++i) {
		if(!is_taken(i)) {
			PrimitivePrimaryTaskStorageItem<T> tmp_item = data.get(i);
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

template <typename TT, template <typename S> class CircularArray>
inline size_t PrimitivePrimaryTaskStorage<TT, CircularArray>::get_length() {
	return bottom - top;
}

template <typename TT, template <typename S> class CircularArray>
inline bool PrimitivePrimaryTaskStorage<TT, CircularArray>::is_empty() {
	clean();
	return bottom > top;
}

template <typename TT, template <typename S> class CircularArray>
inline bool PrimitivePrimaryTaskStorage<TT, CircularArray>::is_full() {
	return (!data.is_growable()) && (bottom - top == data.get_capacity());
}

template <typename TT, template <typename S> class CircularArray>
void PrimitivePrimaryTaskStorage<TT, CircularArray>::clean() {
	while(is_taken(top)) {
		++top;
	}
}

template <typename TT, template <typename S> class CircularArray>
void PrimitivePrimaryTaskStorage<TT, CircularArray>::print_name() {
	std::cout << "PrimitivePrimaryTaskStorage";
}

}

#endif /* PRIMITIVEPRIMARYTASKSTORAGE_H_ */
