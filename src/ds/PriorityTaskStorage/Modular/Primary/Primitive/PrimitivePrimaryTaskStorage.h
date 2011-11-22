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

	PrimitivePrimaryTaskStorage(size_t expected_capacity);
//	PrimitivePrimaryTaskStorage(size_t expected_capacity, PerformanceCounters& perf_count);
	~PrimitivePrimaryTaskStorage();

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

	static void print_name();

private:
	void clean(PerformanceCounters& pc);

	size_t top;
	size_t bottom;

	CircularArray<PrimitivePrimaryTaskStorageItem<T> > data;

//	PerformanceCounters perf_count;

	static const T null_element;
};

template <typename TT, template <typename S> class CircularArray>
const TT PrimitivePrimaryTaskStorage<TT, CircularArray>::null_element = nullable_traits<T>::null_value;

template <typename TT, template <typename S> class CircularArray>
inline PrimitivePrimaryTaskStorage<TT, CircularArray>::PrimitivePrimaryTaskStorage(size_t expected_capacity)
: top(0), bottom(0), data(expected_capacity) {

}
/*
template <typename TT, template <typename S> class CircularArray>
inline PrimitivePrimaryTaskStorage<TT, CircularArray>::PrimitivePrimaryTaskStorage(size_t expected_capacity, PerformanceCounters& perf_count)
: top(0), bottom(0), data(expected_capacity), perf_count(perf_count) {

}*/

template <typename TT, template <typename S> class CircularArray>
inline PrimitivePrimaryTaskStorage<TT, CircularArray>::~PrimitivePrimaryTaskStorage() {

}

template <typename TT, template <typename S> class CircularArray>
typename PrimitivePrimaryTaskStorage<TT, CircularArray>::iterator PrimitivePrimaryTaskStorage<TT, CircularArray>::begin(PerformanceCounters& pc) {
	return top;
}

template <typename TT, template <typename S> class CircularArray>
typename PrimitivePrimaryTaskStorage<TT, CircularArray>::iterator PrimitivePrimaryTaskStorage<TT, CircularArray>::end(PerformanceCounters& pc) {
	return bottom;
}

template <typename TT, template <typename S> class CircularArray>
TT PrimitivePrimaryTaskStorage<TT, CircularArray>::take(iterator item, PerformanceCounters& pc) {
	assert(item < bottom);

	PrimitivePrimaryTaskStorageItem<T>& ptsi = data.get(item);

	if(ptsi.index != item) {
		pc.num_unsuccessful_takes.incr();
		return null_element;
	}
	T ret = ptsi.data;
	BaseStrategy* s = ptsi.s;
	if(!SIZET_CAS(&(ptsi.index), item, item + 1)) {
		pc.num_unsuccessful_takes.incr();
		return null_element;
	}
	delete s;

	pc.num_successful_takes.incr();
	return ret;
}

template <typename TT, template <typename S> class CircularArray>
bool PrimitivePrimaryTaskStorage<TT, CircularArray>::is_taken(iterator item, PerformanceCounters& pc) {
	return data.get(item).index != item;
}

template <typename TT, template <typename S> class CircularArray>
prio_t PrimitivePrimaryTaskStorage<TT, CircularArray>::get_steal_priority(iterator item, PerformanceCounters& pc) {
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
size_t PrimitivePrimaryTaskStorage<TT, CircularArray>::get_task_id(iterator item, PerformanceCounters& pc) {
	return item;
}

template <typename TT, template <typename S> class CircularArray>
template <class Strategy>
inline void PrimitivePrimaryTaskStorage<TT, CircularArray>::push(Strategy& s, T item, PerformanceCounters& pc) {
	pc.push_time.start_timer();
	if(bottom - top == data.get_capacity()) {
		clean(pc);
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
	pc.push_time.stop_timer();
}

template <typename TT, template <typename S> class CircularArray>
inline TT PrimitivePrimaryTaskStorage<TT, CircularArray>::pop(PerformanceCounters& pc) {
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
		ret = take(best, pc);
	} while(ret == null_element);

	pc.num_successful_pops.incr();
	pc.pop_time.stop_timer();
	return ret;
}

template <typename TT, template <typename S> class CircularArray>
inline TT PrimitivePrimaryTaskStorage<TT, CircularArray>::peek(PerformanceCounters& pc) {
	iterator i = begin(pc);
	while(i != end(pc) && is_taken(top, pc)) {
		++i;
	}

	if(i == end(pc)) {
		return null_element;
	}
	top = i;

	iterator best = i;
	PrimitivePrimaryTaskStorageItem<T> ret_item = data.get(i);
	prio_t best_prio = ret_item.pop_prio;
	++i;
	for(; i != end(pc); ++i) {
		if(!is_taken(i, pc)) {
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
inline size_t PrimitivePrimaryTaskStorage<TT, CircularArray>::get_length(PerformanceCounters& pc) {
	return bottom - top;
}

template <typename TT, template <typename S> class CircularArray>
inline bool PrimitivePrimaryTaskStorage<TT, CircularArray>::is_empty(PerformanceCounters& pc) {
	clean(pc);
	return bottom == top;
}

template <typename TT, template <typename S> class CircularArray>
inline bool PrimitivePrimaryTaskStorage<TT, CircularArray>::is_full(PerformanceCounters& pc) {
	return (!data.is_growable()) && (bottom - top == data.get_capacity());
}

template <typename TT, template <typename S> class CircularArray>
void PrimitivePrimaryTaskStorage<TT, CircularArray>::clean(PerformanceCounters& pc) {
	while(top < bottom && is_taken(top, pc)) {
		++top;
	}
}

template <typename TT, template <typename S> class CircularArray>
void PrimitivePrimaryTaskStorage<TT, CircularArray>::print_name() {
	std::cout << "PrimitivePrimaryTaskStorage";
}

}

#endif /* PRIMITIVEPRIMARYTASKSTORAGE_H_ */
