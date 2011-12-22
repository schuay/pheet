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

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
class PrimitiveSecondaryTaskStorage {
public:
	typedef TT T;
	typedef /*struct{
		void print_headers() {}
		void print_values() {}
	}*/ PrimitiveSecondaryTaskStoragePerformanceCounters<Scheduler> PerformanceCounters;

	PrimitiveSecondaryTaskStorage(Primary<Scheduler, TT>* primary);
//	PrimitiveSecondaryTaskStorage(Primary<TT>* primary, PerformanceCounters& perf_count);
	~PrimitiveSecondaryTaskStorage();

	TT steal(typename Scheduler::StealerDescriptor& sd, typename Primary<Scheduler, TT>::PerformanceCounters& ppc, PerformanceCounters& pc);

	static void print_name();

private:
	Primary<Scheduler, TT>* primary;

//	PerformanceCounters perf_count;

	static T const null_element;
};

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
TT const PrimitiveSecondaryTaskStorage<Scheduler, TT, Primary>::null_element = nullable_traits<TT>::null_value;

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<Scheduler, TT, Primary>::PrimitiveSecondaryTaskStorage(Primary<Scheduler, T>* primary)
: primary(primary) {

}
/*
template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<Scheduler, TT, Primary>::PrimitiveSecondaryTaskStorage(Primary<Scheduler, T>* primary, PerformanceCounters& perf_count)
: primary(primary), perf_count(perf_count) {

}*/

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<Scheduler, TT, Primary>::~PrimitiveSecondaryTaskStorage() {

}

template <class Scheduler, typename TT, template <class Sched, typename S> class Primary>
TT PrimitiveSecondaryTaskStorage<Scheduler, TT, Primary>::steal(typename Scheduler::StealerDescriptor& sd, typename Primary<Scheduler, TT>::PerformanceCounters& ppc, PerformanceCounters& pc) {
	pc.steal_time.start_timer();
	typename Primary<Scheduler, T>::iterator begin = primary->begin(ppc);
	typename Primary<Scheduler, T>::iterator end = primary->end(ppc);
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
void PrimitiveSecondaryTaskStorage<Scheduler, TT, Primary>::print_name() {
	std::cout << "PrimitiveSecondaryTaskStorage";
}

}

#endif /* PRIMITIVESECONDARYTASKSTORAGE_H_ */
