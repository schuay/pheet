/*
 * KLSMLocalityTaskStorage.h
 *
 *  Created on: Sep 18, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGE_H_
#define KLSMLOCALITYTASKSTORAGE_H_

#include "KLSMLocalityTaskStoragePlace.h"

#include <atomic>

namespace pheet {

template <class Pheet, class Strategy>
class KLSMLocalityTaskStorage : public Pheet::Scheduler::BaseTaskStorage {
public:
	typedef KLSMLocalityTaskStorage<Pheet, Strategy> Self;
	typedef typename Strategy::BaseStrategy::TaskStorage ParentTaskStorage;
	typedef typename ParentTaskStorage::BaseItem BaseItem;
	typedef typename BaseItem::T T;
	typedef KLSMLocalityTaskStoragePlace<Pheet, Self, typename ParentTaskStorage::Place, Strategy> Place;

	KLSMLocalityTaskStorage(ParentTaskStorage* parent)
	:parent(parent) {
		procs_t num_places = Pheet::get_num_places();
		places = new std::atomic<Place*>[num_places];
		for(procs_t i = 0; i < num_places; ++i) {
			places[i].store(nullptr, std::memory_order_relaxed);
		}
	}

	~KLSMLocalityTaskStorage() {
		delete[] places;
		singleton = nullptr;
	}

	/*
	 * Called by each place exactly once. Only one place will get created == true
	 * This place is responsible for cleanup of base task storage
	 */
	static Self* get(Place* place, ParentTaskStorage* parent, bool& created) {
		Self* ret = singleton.load(std::memory_order_acquire);
		if(ret == nullptr) {
			// We try to create a task storage
			ret = new Self(parent);

			Self* expected = nullptr;
			if(singleton.compare_exchange_strong(expected, ret, std::memory_order_acq_rel, std::memory_order_relaxed)) {
				created = true;
			}
			else {
				// Conflict on initialization, need to delete doubly initialized object
				created = false;
				delete ret;
				pheet_assert(expected != nullptr);
				ret = expected;
			}
		}
		else {
			created = false;
		}
		ret->places[Pheet::get_place_id()].store(place, std::memory_order_release);
		return ret;
	}

	/*
	 * Override from base class
	 */
	T pop(BaseItem* boundary, procs_t place_id) {
		// Should only be called by the owner
		pheet_assert(place_id == Pheet::get_place_id());

		// Load place.
		Place* place = places[place_id].load(std::memory_order_relaxed);
		// Should already be filled, otherwise there would be no pop
		pheet_assert(place != nullptr);

		return place->pop(boundary);
	}

	/*
	 * Override from base class
	 */
	T steal(BaseItem* boundary, procs_t place_id) {
		// Should be called by everyone except the owner
		pheet_assert(place_id != Pheet::get_place_id());

		// We are stealing from a foreign place, therefore acquire synchronization is needed
		Place* place = places[place_id].load(std::memory_order_acquire);
		// Place should be filled, otherwise there would be nothing to steal from it
		pheet_assert(place != nullptr);

		return place->steal(boundary);
	}

private:
	static std::atomic<Self*> singleton;

	ParentTaskStorage* parent;

	std::atomic<Place*>* places;
};

template <class Pheet, class Strategy>
std::atomic<KLSMLocalityTaskStorage<Pheet, Strategy>*>
KLSMLocalityTaskStorage<Pheet, Strategy>::singleton(nullptr);

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGE_H_ */