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

#include "PrimitiveHeapPrimaryTaskStoragePerformanceCounters.h"
#include "../../../../PriorityQueue/STLPriorityQueueWrapper/STLPriorityQueueWrapper.h"

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

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT = STLPriorityQueueWrapper>
class PrimitiveHeapPrimaryTaskStorage {
public:
	typedef TT T;
	// Not completely a standard iterator, as it doesn't support a dereference operation, but this makes implementation simpler for now (and even more lightweight)
	typedef size_t iterator;
	typedef PrimitiveHeapPrimaryTaskStoragePerformanceCounters PerformanceCounters;
	typedef PriorityQueueT<iterator, PrimitiveHeapPrimaryTaskStorageComparator<CircularArray<PrimitiveHeapPrimaryTaskStorageItem<T> > > > PriorityQueue;

	PrimitiveHeapPrimaryTaskStorage(size_t initial_capacity);
//	PrimitiveHeapPrimaryTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count);
	~PrimitiveHeapPrimaryTaskStorage();

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
	T local_take(iterator item, PerformanceCounters& pc);
	void clean_pq(PerformanceCounters& pc);
	void clean(PerformanceCounters& pc);

	size_t top;
	size_t bottom;

	CircularArray<PrimitiveHeapPrimaryTaskStorageItem<T> > data;
	PriorityQueue pq;

//	PerformanceCounters perf_count;

	static const T null_element;
};

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
const TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::null_element = nullable_traits<T>::null_value;

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
inline PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::PrimitiveHeapPrimaryTaskStorage(size_t expected_capacity)
: top(0), bottom(0), data(expected_capacity),
  pq(PrimitiveHeapPrimaryTaskStorageComparator<CircularArray<PrimitiveHeapPrimaryTaskStorageItem<T> > >(&data)) {

}
/*
template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
inline PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::PrimitiveHeapPrimaryTaskStorage(size_t expected_capacity, PerformanceCounters& perf_count)
: top(0), bottom(0), data(expected_capacity),
  heap(PrimitiveHeapPrimaryTaskStorageComparator<CircularArray<PrimitiveHeapPrimaryTaskStorageItem<T> > >(&data)),
  perf_count(perf_count) {

}*/

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
inline PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::~PrimitiveHeapPrimaryTaskStorage() {

}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
typename PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::iterator PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::begin(PerformanceCounters& pc) {
	return top;
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
typename PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::iterator PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::end(PerformanceCounters& pc) {
	return bottom;
}

/*
 * Same as take, but is only safe to be called by the owning thread (but is faster)
 */
template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::local_take(iterator item, PerformanceCounters& pc) {
	assert(item < bottom);

	PrimitiveHeapPrimaryTaskStorageItem<T>& ptsi = data.get(item);

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

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::take(iterator item, PerformanceCounters& pc) {
	assert(item < bottom);

	PrimitiveHeapPrimaryTaskStorageItem<T>& ptsi = data.get(item);

	if(ptsi.index != item) {
		pc.num_unsuccessful_takes.incr();
		return null_element;
	}
	// Make sure we really get the current item
	MEMORY_FENCE();

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

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
bool PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::is_taken(iterator item, PerformanceCounters& pc) {
	return data.get(item).index != item;
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
prio_t PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::get_steal_priority(iterator item, PerformanceCounters& pc) {
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
template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
size_t PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::get_task_id(iterator item, PerformanceCounters& pc) {
	return item;
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
template <class Strategy>
inline void PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::push(Strategy& s, T item, PerformanceCounters& pc) {
	pc.push_time.start_timer();
	if(bottom - top == data.get_capacity()) {
		clean(pc);
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
	pq.push(bottom);

	MEMORY_FENCE();

	bottom++;
	pc.push_time.stop_timer();
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
inline TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::pop(PerformanceCounters& pc) {
	pc.total_size_pop.add(pq.get_length());
	pc.pop_time.start_timer();
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
		if(pq.is_empty()) {
			pc.num_unsuccessful_pops.incr();
			pc.pop_time.stop_timer();
			return null_element;
		}

		size_t el = pq.pop();

		ret = local_take(el, pc);
	} while(ret == null_element);

	clean(pc);

	pc.num_successful_pops.incr();
	pc.pop_time.stop_timer();
	return ret;
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
inline TT PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::peek(PerformanceCounters& pc) {
/*	iterator i = begin();
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
	}*/

	if(pq.is_empty()) {
		return null_element;
	}
	size_t el = pq.pop();
	TT ret;
	while(is_taken(el, pc)) {
		if(pq.is_empty()) {
			return null_element;
		}
		el = pq.pop();
	}
	ret = data.get(el);

	return ret;
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
inline size_t PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::get_length(PerformanceCounters& pc) {
	clean(pc);
	return bottom - top;
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
inline bool PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::is_empty(PerformanceCounters& pc) {
	clean_pq(pc);
	return pq.is_empty();
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
inline bool PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::is_full(PerformanceCounters& pc) {
	clean(pc);
	return (!data.is_growable()) && (bottom - top == data.get_capacity());
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
void PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::clean_pq(PerformanceCounters& pc) {
	if(pq.is_empty()) {
		return;
	}
	size_t el = pq.peek();
	while(is_taken(el, pc)) {
		pq.pop();
		if(pq.is_empty()) {
			return;
		}
		el = pq.peek();
	}
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
void PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::clean(PerformanceCounters& pc) {
	size_t i = top;
	while(i < bottom && is_taken(i, pc)) {
		++i;
	}
	top = i;
}

template <typename TT, template <typename S> class CircularArray, template <typename S, typename Comp> class PriorityQueueT>
void PrimitiveHeapPrimaryTaskStorage<TT, CircularArray, PriorityQueueT>::print_name() {
	std::cout << "PrimitiveHeapPrimaryTaskStorage<";
	PriorityQueue::print_name();
	std::cout << ">";
}

}

#endif /* PRIMITIVEHEAPPRIMARYTASKSTORAGE_H_ */
