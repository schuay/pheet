/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef PARETOLOCALITYTASKSTORAGE_H_
#define PARETOLOCALITYTASKSTORAGE_H_

#include "ParetoLocalityTaskStoragePlace.h"

namespace pheet
{

template <class Pheet, class Strategy>
class ParetoLocalityTaskStorage : public Pheet::Scheduler::BaseTaskStorage
{
public:
	typedef ParetoLocalityTaskStorage<Pheet, Strategy> Self;
	typedef typename Strategy::BaseStrategy::TaskStorage ParentTaskStorage;
	typedef typename ParentTaskStorage::BaseItem BaseItem;
	typedef typename BaseItem::T T;
	typedef ParetoLocalityTaskStoragePlace<Pheet, Self, typename ParentTaskStorage::Place, Strategy>
	Place;

	ParetoLocalityTaskStorage(ParentTaskStorage* parent);
	~ParetoLocalityTaskStorage();

	/*
	 * Called by each place exactly once. Only one place will get created == true
	 * This place is responsible for cleanup of base task storage
	 */
	static Self* get(Place* place, ParentTaskStorage* parent, bool& created);

	/*
	 * Override from base class
	 */
	T pop(BaseItem* boundary, procs_t place_id);
	/*
	 * Override from base class
	 */
	T steal(BaseItem* boundary, procs_t place_id);

	static void print_name();

private:
	static std::atomic<Self*> singleton;
	ParentTaskStorage* parent;
	std::atomic<Place*>* places;
};

template <class Pheet, class Strategy>
ParetoLocalityTaskStorage<Pheet, Strategy>::
ParetoLocalityTaskStorage(ParentTaskStorage* parent)
	: parent(parent)
{
	procs_t num_places = Pheet::get_num_places();
	places = new std::atomic<Place*>[num_places];
	for (procs_t i = 0; i < num_places; ++i) {
		places[i].store(nullptr, std::memory_order_relaxed);
	}
}

template <class Pheet, class Strategy>
ParetoLocalityTaskStorage<Pheet, Strategy>::
~ParetoLocalityTaskStorage()
{
	delete[] places;
	singleton = nullptr;
}

template <class Pheet, class Strategy>
typename ParetoLocalityTaskStorage<Pheet, Strategy>::Self*
ParetoLocalityTaskStorage<Pheet, Strategy>::
get(Place* place, ParentTaskStorage* parent, bool& created)
{
	Self* ret = singleton.load(std::memory_order_acquire);
	if (ret == nullptr) {
		// We try to create a task storage
		ret = new Self(parent);

		Self* expected = nullptr;
		if (singleton.compare_exchange_strong(expected, ret, std::memory_order_acq_rel,
		                                      std::memory_order_relaxed)) {
			created = true;
		} else {
			// Conflict on initialization, need to delete doubly initialized object
			created = false;
			delete ret;
			pheet_assert(expected != nullptr);
			ret = expected;
		}
	} else {
		created = false;
	}
	ret->places[Pheet::get_place_id()].store(place, std::memory_order_release);
	return ret;
}

template <class Pheet, class Strategy>
typename ParetoLocalityTaskStorage<Pheet, Strategy>::T
ParetoLocalityTaskStorage<Pheet, Strategy>::
pop(BaseItem* boundary, procs_t place_id)
{
	// Should only be called by the owner
	pheet_assert(place_id == Pheet::get_place_id());

	// Load place.
	Place* place = places[place_id].load(std::memory_order_relaxed);
	// Should already be filled, otherwise there would be no pop
	pheet_assert(place != nullptr);
	pheet_assert(place == Pheet::get_place()->template get_task_storage<Strategy>());

	return place->pop(boundary);
}

template <class Pheet, class Strategy>
typename ParetoLocalityTaskStorage<Pheet, Strategy>::T
ParetoLocalityTaskStorage<Pheet, Strategy>::
steal(BaseItem* boundary, procs_t place_id)
{
	// Should be called by everyone except the owner
	pheet_assert(place_id != Pheet::get_place_id());

	// stealing is called in the context of the stealer, therefore need to retrieve place
	// from Scheduler (might not yet have been initialized)
	Place* place = Pheet::get_place()->template get_task_storage<Strategy>();
	// Place should be created if it does not exist
	pheet_assert(place != nullptr);
	pheet_assert(place == places[Pheet::get_place_id()]);

	return place->steal(boundary);

}

template <class Pheet, class Strategy>
void
ParetoLocalityTaskStorage<Pheet, Strategy>::
print_name()
{
	std::cout << "ParetoLocalityTaskStorage";
}

template <class Pheet, class Strategy>
std::atomic<ParetoLocalityTaskStorage<Pheet, Strategy>*>
ParetoLocalityTaskStorage<Pheet, Strategy>::singleton(nullptr);

} /* namespace pheet */

#endif /* PARETOLOCALITYTASKSTORAGE_H_ */
