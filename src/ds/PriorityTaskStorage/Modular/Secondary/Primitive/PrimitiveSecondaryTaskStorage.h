/*
 * PrimitiveSecondaryTaskStorage.h
 *
 *  Created on: 21.09.2011
 *      Author: mwimmer
 */

#ifndef PRIMITIVESECONDARYTASKSTORAGE_H_
#define PRIMITIVESECONDARYTASKSTORAGE_H_

namespace pheet {

template <typename TT, template <typename S> class Primary>
class PrimitiveSecondaryTaskStorage {
public:
	typedef TT T;

	PrimitiveSecondaryTaskStorage(Primary<TT>* primary);
	PrimitiveSecondaryTaskStorage(Primary<TT>* primary, BasicPerformanceCounter<stealing_deque_count_steals>& num_stolen);
	~PrimitiveSecondaryTaskStorage();

	TT steal();

private:
	Primary<TT>* primary;

	BasicPerformanceCounter<stealing_deque_count_steals> num_stolen;

	static T const null_element;
};

template <typename TT, template <typename S> class Primary>
TT const PrimitiveSecondaryTaskStorage<TT, Primary>::null_element = nullable_traits<TT>::null_value;

template <typename TT, template <typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<TT, Primary>::PrimitiveSecondaryTaskStorage(Primary<T>* primary)
: primary(primary) {

}

template <typename TT, template <typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<TT, Primary>::PrimitiveSecondaryTaskStorage(Primary<T>* primary, BasicPerformanceCounter<stealing_deque_count_steals>& num_stolen)
: primary(primary), num_stolen(num_stolen) {

}

template <typename TT, template <typename S> class Primary>
inline PrimitiveSecondaryTaskStorage<TT, Primary>::~PrimitiveSecondaryTaskStorage() {

}

template <typename TT, template <typename S> class Primary>
TT PrimitiveSecondaryTaskStorage<TT, Primary>::steal() {
	typename Primary<T>::iterator best;
	prio_t best_prio = 0;

	for(typename Primary<T>::iterator i = primary->begin(); i != primary->end(); ++i) {
		if(!primary->is_taken(i)) {
			prio_t tmp_prio = primary->get_steal_priority(i);
			if(tmp_prio > best_prio) {
				best = i;
				best_prio = tmp_prio;
			}
		}
	}

	if(best_prio == 0) {
		return null_element;
	}
	// We don't care if taking fails, this is just stealing, which is allowed to fail
	return primary->take(best);
}

}

#endif /* PRIMITIVESECONDARYTASKSTORAGE_H_ */
