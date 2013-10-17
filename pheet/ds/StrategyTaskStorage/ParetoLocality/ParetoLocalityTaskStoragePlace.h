/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef PARETOLOCALITYTASKSTORAGEPLACE_H_
#define PARETOLOCALITYTASKSTORAGEPLACE_H_

#include "ParetoLocalityTaskStorageItem.h"
#include "pheet/misc/type_traits.h"
#include <mutex>
#include <vector>


namespace pheet
{

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
class ParetoLocalityTaskStoragePlace : public ParentTaskStoragePlace::BaseTaskStoragePlace
{
public:
	typedef ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy> Self;
	typedef typename ParentTaskStoragePlace::BaseItem BaseItem;
	typedef ParetoLocalityTaskStorageItem<Pheet, Self, BaseItem, Strategy> Item;
	typedef typename BaseItem::T T;

	ParetoLocalityTaskStoragePlace(ParentTaskStoragePlace* parent_place);
	~ParetoLocalityTaskStoragePlace();

	void push(Strategy&& strategy, T data);
	T pop(BaseItem* boundary);
	T steal(BaseItem* boundary);
	void clean_up();

private:
	ParentTaskStoragePlace* parent_place;
	TaskStorage* task_storage;
	bool created_task_storage;
	std::vector<Item*> items;
	std::mutex mutex;
};

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
ParetoLocalityTaskStoragePlace(ParentTaskStoragePlace* parent_place)
	: parent_place(parent_place)
{
	task_storage = TaskStorage::get(this, parent_place->get_central_task_storage(),
	                                created_task_storage);
}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
~ParetoLocalityTaskStoragePlace()
{
	if (created_task_storage) {
		delete task_storage;
	}
}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
void
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
push(Strategy&& strategy, T data)
{
	std::lock_guard<std::mutex> lock(mutex);
	Item* item = new Item();
	item->owner = this;
	item->strategy = std::move(strategy);
	item->data = data;
	item->task_storage = task_storage;
	items.push_back(item);
	item->taken.store(false, std::memory_order_release);

	parent_place->push(item);
}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
typename ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::T
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
pop(BaseItem* boundary)
{
	std::lock_guard<std::mutex> lock(mutex);
	if (!items.empty()) {
		Item* item = items.front();
		items.erase(items.begin());
		return item->data;
	}
	return nullable_traits<T>::null_value;
}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
typename ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::T
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
steal(BaseItem* boundary)
{
	return nullable_traits<T>::null_value;
}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
void
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
clean_up()
{

}

} /* namepsace pheet */

#endif /* PARETOLOCALITYTASKSTORAGEPLACE_H_ */
