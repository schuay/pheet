/*
 * PrimitiveHeapPrimaryTaskStorage.h
 *
 *  Created on: 21.09.2011
 *      Author: mwimmer
 */

#ifndef PRIMITIVEHEAPPRIMARYTASKSTORAGE_H_
#define PRIMITIVEHEAPPRIMARYTASKSTORAGE_H_

#include "../../../../../sched/strategies/BaseStrategy.h"
#include <vector>
#include <queue>

namespace pheet {

template <typename TT>
struct PrimitiveHeapPrimaryTaskStorageItem {
	TT data;
	BaseStrategy* s;
	prio_t pop_prio;
	// We won't be able to just store the steal prio statically in the future,
	// as there might be more types of steal priorities, and calculating those may be expensive!
	prio_t steal_prio;
	size_t index;
};

template <class CircularArray>
class PrimitiveHeapPrimaryTaskStorageComparator {
public:
	PrimitiveHeapPrimaryTaskStorageComparator(CircularArray* data)
	: data(data) {};

	bool operator() (size_t lhs, size_t rhs) const
	{
		return data->get(lhs).pop_prio < data->get(rhs).pop_prio;
	}

private:
	CircularArray* data;
};

template <typename TT, template <typename S> class CircularArray>
class PrimitiveHeapPrimaryTaskStorage {
public:
	typedef TT T;
	// Not completely a standard iterator, as it doesn't support a dereference operation, but this makes implementation simpler for now (and even more lightweight)
	typedef size_t iterator;
	typedef struct{
		void print_headers() {}
		void print_values() {}
	} PerformanceCounters;

	PrimitiveHeapPrimaryTaskStorage(size_t initial_capacity);
	PrimitiveHeapPrimaryTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count);
	~PrimitiveHeapPrimaryTaskStorage();

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

	CircularArray<PrimitiveHeapPrimaryTaskStorageItem<T> > data;
	std::priority_queue<iterator, std::vector<iterator>, PrimitiveHeapPrimaryTaskStorageComparator<CircularArray<PrimitiveHeapPrimaryTaskStorageItem<T> > > > heap;

	PerformanceCounters perf_count;

	static const T null_element;
};

template <typename TT, template <typename S> class CircularArray>
const TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::null_element = nullable_traits<T>::null_value;

template <typename TT, template <typename S> class CircularArray>
inline PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::PrimitiveHeapPrimaryTaskStorage(size_t initial_capacity)
: top(0), bottom(0), data(initial_capacity) {

}

template <typename TT, template <typename S> class CircularArray>
inline PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::PrimitiveHeapPrimaryTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count)
: top(0), bottom(0), data(initial_capacity),
  heap(PrimitiveHeapPrimaryTaskStorageComparator<CircularArray<PrimitiveHeapPrimaryTaskStorageItem<T> > >(&data)),
  perf_count(perf_count) {

}

template <typename TT, template <typename S> class CircularArray>
inline PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::~PrimitiveHeapPrimaryTaskStorage() {

}

template <typename TT, template <typename S> class CircularArray>
typename PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::iterator PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::begin() {
	return top;
}

template <typename TT, template <typename S> class CircularArray>
typename PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::iterator PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::end() {
	return bottom;
}

template <typename TT, template <typename S> class CircularArray>
TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::take(iterator item) {
	assert(item < bottom);

	PrimitiveHeapPrimaryTaskStorageItem<T>& ptsi = data.get(item);

	if(ptsi.index != item) {
		return null_element;
	}
	T ret = ptsi.data;
	BaseStrategy* s = ptsi.s;
	if(!SIZET_CAS(&(ptsi.index), item, item + 1)) {
		return null_element;
	}
	delete s;

	return ret;
}

template <typename TT, template <typename S> class CircularArray>
bool PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::is_taken(iterator item) {
	return data.get(item).index != item;
}

template <typename TT, template <typename S> class CircularArray>
prio_t PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::get_steal_priority(iterator item) {
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
size_t PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::get_task_id(iterator item) {
	return item;
}

template <typename TT, template <typename S> class CircularArray>
template <class Strategy>
inline void PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::push(Strategy& s, T item) {
	if(bottom - top == data.get_capacity()) {
		clean();
		if(bottom - top == data.get_capacity()) {
			assert(data.is_growable());
			data.grow(bottom, top);
		}
	}

	PrimitiveHeapPrimaryTaskStorageItem<TT> to_put;
	to_put.data = item;
	to_put.s = new Strategy(s);
	to_put.pop_prio = s.get_pop_priority(bottom);
	to_put.steal_prio = s.get_steal_priority(bottom);
	to_put.index = bottom;

	data.put(bottom, to_put);
	heap.push(bottom);

	MEMORY_FENCE();

	bottom++;
}

template <typename TT, template <typename S> class CircularArray>
inline TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::pop() {
	T ret;
/*	do {
		iterator i = begin();
		while(i != end() && is_taken(i)) {
			++i;
		}
		top = i;

		if(i == end()) {
			return null_element;
		}

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
	} while(ret == null_element);*/
	do {
		if(heap.empty()) {
			return null_element;
		}

		size_t el = heap.top();
		heap.pop();

		ret = take(el);
	} while(ret == null_element);

	return ret;
}

template <typename TT, template <typename S> class CircularArray>
inline TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::peek() {
	iterator i = begin();
	while(i != end() && is_taken(top)) {
		++i;
	}

	if(i == end()) {
		return null_element;
	}
	top = i;

	iterator best = i;
	PrimitiveHeapPrimaryTaskStorageItem<T> ret_item = data.get(i);
	prio_t best_prio = ret_item.pop_prio;
	++i;
	for(; i != end(); ++i) {
		if(!is_taken(i)) {
			PrimitiveHeapPrimaryTaskStorageItem<T> tmp_item = data.get(i);
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
inline size_t PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::get_length() {
	return bottom - top;
}

template <typename TT, template <typename S> class CircularArray>
inline bool PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::is_empty() {
	clean();
	return bottom > top;
}

template <typename TT, template <typename S> class CircularArray>
inline bool PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::is_full() {
	return (!data.is_growable()) && (bottom - top == data.get_capacity());
}

template <typename TT, template <typename S> class CircularArray>
void PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::clean() {
	while(is_taken(top)) {
		++top;
	}
}

template <typename TT, template <typename S> class CircularArray>
void PrimitiveHeapPrimaryTaskStorage<TT, CircularArray>::print_name() {
	std::cout << "PrimitiveHeapPrimaryTaskStorage";
}

}

#endif /* PRIMITIVEHEAPPRIMARYTASKSTORAGE_H_ */
