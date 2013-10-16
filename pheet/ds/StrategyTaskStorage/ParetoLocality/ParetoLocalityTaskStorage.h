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
}

template <class Pheet, class Strategy>
typename ParetoLocalityTaskStorage<Pheet, Strategy>::Self*
ParetoLocalityTaskStorage<Pheet, Strategy>::
get(Place* place, ParentTaskStorage* parent, bool& created)
{
	return nullptr;
}

template <class Pheet, class Strategy>
typename ParetoLocalityTaskStorage<Pheet, Strategy>::T
ParetoLocalityTaskStorage<Pheet, Strategy>::
pop(BaseItem* boundary, procs_t place_id)
{
	return nullptr;
}

template <class Pheet, class Strategy>
typename ParetoLocalityTaskStorage<Pheet, Strategy>::T
ParetoLocalityTaskStorage<Pheet, Strategy>::
steal(BaseItem* boundary, procs_t place_id)
{
	return nullptr;
}

template <class Pheet, class Strategy>
void
ParetoLocalityTaskStorage<Pheet, Strategy>::
print_name()
{
	std::cout << "ParetoLocalityTaskStorage";
}


} /* namespace pheet */

#endif /* PARETOLOCALITYTASKSTORAGE_H_ */
