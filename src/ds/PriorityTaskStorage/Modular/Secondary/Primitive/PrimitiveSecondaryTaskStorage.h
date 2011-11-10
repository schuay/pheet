/*
 * PrimitiveSecondaryTaskStorage.h
 *
 *  Created on: 21.09.2011
 *      Author: mwimmer
 */

#ifndef PRIMITIVESECONDARYTASKSTORAGE_H_
#define PRIMITIVESECONDARYTASKSTORAGE_H_

#include "PrimitiveSecondaryTaskStoragePerformanceCounters.h"

namespace pheet {

template <typename TT, template <typename S> class Primary>
class PrimitiveSecondaryTaskStorage {
public:
	typedef TT T;
	typedef /*struct{
		void print_headers() {}
		void print_values() {}
	}*/ PrimitiveSecondaryTaskStoragePerformanceCounters PerformanceCounters;

	PrimitiveSecondaryTaskStorage(Primary<TT>* primary);
//	PrimitiveSecondaryTaskStorage(Primary<TT>* primary, PerformanceCounters& perf_count);
	~PrimitiveSecondaryTaskStorage();

	TT steal(typename Primary<TT>::PerformanceCounters& ppc, PerformanceCounters& pc);

	static void print_name();

private:
	Primary<TT>* primary;

//	PerformanceCounters perf_count;

	static T const null_element;
};

template <typename TT, template <typename S> class Primary>
TT const PrimitiveSecondaryTaskStorage<TT, Primary>::null_element = nullable_traits<TT>::null_value;

template <typename TT, template <typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<TT, Primary>::PrimitiveSecondaryTaskStorage(Primary<T>* primary)
: primary(primary) {

}
/*
template <typename TT, template <typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<TT, Primary>::PrimitiveSecondaryTaskStorage(Primary<T>* primary, PerformanceCounters& perf_count)
: primary(primary), perf_count(perf_count) {

}*/

template <typename TT, template <typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<TT, Primary>::~PrimitiveSecondaryTaskStorage() {

}

template <typename TT, template <typename S> class Primary>
TT PrimitiveSecondaryTaskStorage<TT, Primary>::steal(typename Primary<TT>::PerformanceCounters& ppc, PerformanceCounters& pc) {
	pc.steal_time.start_timer();
	typename Primary<T>::iterator begin = primary->begin(ppc);
	typename Primary<T>::iterator end = primary->end(ppc);
	pc.total_size_steal.add(end - begin);
	typename Primary<T>::iterator best;
	prio_t best_prio = 0;

	for(typename Primary<T>::iterator i = begin; i != end; ++i) {
		if(!primary->is_taken(i, ppc)) {
			prio_t tmp_prio = primary->get_steal_priority(i, ppc);
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

template <typename TT, template <typename S> class Primary>
void PrimitiveSecondaryTaskStorage<TT, Primary>::print_name() {
	std::cout << "PrimitiveSecondaryTaskStorage";
}

}

#endif /* PRIMITIVESECONDARYTASKSTORAGE_H_ */
