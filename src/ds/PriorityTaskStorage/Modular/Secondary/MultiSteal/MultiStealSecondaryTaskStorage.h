/*
 * MultiStealSecondaryTaskStorage.h
 *
 *  Created on: 07.01.2012
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef MULTISTEALSECONDARYTASKSTORAGE_H_
#define MULTISTEALSECONDARYTASKSTORAGE_H_

#include "MultiStealSecondaryTaskStoragePerformanceCounters.h"
#include "../../../../DePriorityQueue/DeHeap/DeHeap.h"

namespace pheet {

template <class Iter>
struct MultiStealSecondaryTaskStorageHeapElement {
	Iter iter;
	size_t steal_prio;
}

template <class HeapEl>
class MultiStealSecondaryTaskStorageComparator {
public:
	MultiStealSecondaryTaskStorageComparator() {};

	bool operator() (HeapEl const& lhs, HeapEl const& rhs) const
	{
		return lhs.steal_prio < rhs.steal_prio;
	}
};

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
class MultiStealSecondaryTaskStorage {
public:
	typedef TT T;
	typedef /*struct{
		void print_headers() {}
		void print_values() {}
	}*/ MultiStealSecondaryTaskStoragePerformanceCounters<Scheduler> PerformanceCounters;
	typedef MultiStealSecondaryTaskStorageHeapElement<typename Primary<Scheduler, T>::iterator> HeapElement;
	typedef MultiStealSecondaryTaskStorageComparator<HeapElement> Comparator;

	MultiStealSecondaryTaskStorage(Primary<Scheduler, TT>* primary, size_t expected_capacity);
//	MultiStealSecondaryTaskStorage(Primary<TT>* primary, PerformanceCounters& perf_count);
	~MultiStealSecondaryTaskStorage();

	TT steal(typename Scheduler::StealerDescriptor& sd, typename Primary<Scheduler, TT>::PerformanceCounters& ppc, PerformanceCounters& pc);
	TT steal_push(Primary<Scheduler, TT>& other_primary, typename Scheduler::StealerDescriptor& sd, typename Primary<Scheduler, TT>::PerformanceCounters& ppc, PerformanceCounters& pc);

	static void print_name();

private:
	Primary<Scheduler, TT>* primary;
	size_t expected_capacity

//	PerformanceCounters perf_count;

	static T const null_element;
};

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
TT const MultiStealSecondaryTaskStorage<Scheduler, TT, Primary>::null_element = nullable_traits<TT>::null_value;

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
inline MultiStealSecondaryTaskStorage<Scheduler, TT, Primary>::MultiStealSecondaryTaskStorage(Primary<Scheduler, T>* primary, size_t expected_capacity)
: primary(primary), expected_capacity(expected_capacity) {

}
/*
template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
inline MultiStealSecondaryTaskStorage<Scheduler, TT, Primary>::MultiStealSecondaryTaskStorage(Primary<Scheduler, T>* primary, PerformanceCounters& perf_count)
: primary(primary), perf_count(perf_count) {

}*/

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
inline MultiStealSecondaryTaskStorage<Scheduler, TT, Primary>::~MultiStealSecondaryTaskStorage() {

}

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
TT MultiStealSecondaryTaskStorage<Scheduler, TT, Primary>::steal(typename Scheduler::StealerDescriptor& sd, typename Primary<Scheduler, TT>::PerformanceCounters& ppc, PerformanceCounters& pc) {
	pc.steal_time.start_timer();
	typename Primary<Scheduler, T>::iterator begin = primary->begin(ppc);
	typename Primary<Scheduler, T>::iterator end = primary->end(ppc);

	// If this happens we probably have invalid iterators
	// We might change this assertion to use some number dependent on ptrdiff_t max, but for now this is better for debugging
	assert(end - begin < 0xFFFFFFF);

	pc.total_size_steal.add(end - begin);
	// g++ 4.4 unfortunately generates a compiler warning for this. Just ignore it, 4.6.1 seems to be more intelligent.
	typename Primary<Scheduler, T>::iterator best;
	prio_t best_prio = 0;

	for(typename Primary<Scheduler, T>::iterator i = begin; i != end; ++i) {
		if(!primary->is_taken(i, ppc)) {
			prio_t tmp_prio = primary->get_steal_priority(i, sd, ppc);
			if(tmp_prio > best_prio) {
				best = i;
				best_prio = tmp_prio;
			}
		}
	}

	if(best_prio == 0) {
		pc.steal_time.stop_timer();
		pc.num_unsuccessful_steals.incr();
		return null_element;
	}
	// We don't care if taking fails, this is just stealing, which is allowed to fail
	TT ret = primary->take(best, ppc);
	if(ret != null_element) {
		pc.num_stolen.incr();
		pc.num_successful_steals.incr();
	}
	else {
		pc.num_unsuccessful_steals.incr();
	}
	pc.steal_time.stop_timer();
	return ret;
}

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
TT MultiStealSecondaryTaskStorage<Scheduler, TT, Primary>::steal_push(Primary<Scheduler, TT>& other_primary, typename Scheduler::StealerDescriptor& sd, typename Primary<Scheduler, TT>::PerformanceCounters& ppc, PerformanceCounters& pc) {
	pc.steal_time.start_timer();
	typename Primary<Scheduler, T>::iterator begin = primary->begin(ppc);
	typename Primary<Scheduler, T>::iterator end = primary->end(ppc);

	// If this happens we probably have invalid iterators
	// We might change this assertion to use some number dependent on ptrdiff_t max, but for now this is better for debugging
	assert(end - begin < 0xFFFFFFF);

	pc.total_size_steal.add(end - begin);

	size_t const threshold = expected_capacity;
	DeHeap<HeapElement, Comparator> heap;
	size_t num_dropped = 0;

	for(typename Primary<Scheduler, T>::iterator i = begin; i != end; ++i) {
		if(!primary->is_taken(i, ppc)) {
			HeapElement he;
			he.steal_prio = primary->get_steal_priority(i, sd, ppc);
			if(heap.get_length() < expected_capacity) {
				he.iter = i;
				heap.push(he);
			}
			else if(heap.min().steal_prio < he.steal_prio) {
				++num_dropped;
				he.iter = i;
				heap.replace_min(he);
			}
		}
	}

	size_t total_found = num_dropped + heap.get_length();
	if(heap.length == 0) {
			pc.steal_time.stop_timer();
			pc.num_unsuccessful_steals.incr();
			return null_element;
		}
	size_t stolen = 0;
	typename Primary<Scheduler, T>::iterator prev;

	while(heap.get_length() != 0 && stolen <= (total_found >> 1)) {
		HeapElement he = heap.pop_max();
	}


	// We don't care if taking fails, this is just stealing, which is allowed to fail
	TT ret = primary->take(best, ppc);
	if(ret != null_element) {
		pc.num_stolen.incr();
		pc.num_successful_steals.incr();
	}
	else {
		pc.num_unsuccessful_steals.incr();
	}
	pc.steal_time.stop_timer();
	return ret;
}

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
void MultiStealSecondaryTaskStorage<Scheduler, TT, Primary>::print_name() {
	std::cout << "MultiStealSecondaryTaskStorage";
}

}

#endif /* MULTISTEALSECONDARYTASKSTORAGE_H_ */
