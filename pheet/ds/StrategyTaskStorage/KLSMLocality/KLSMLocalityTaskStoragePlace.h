/*
 * KLSMLocalityTaskStoragePlace.h
 *
 *  Created on: Sep 18, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGEPLACE_H_
#define KLSMLOCALITYTASKSTORAGEPLACE_H_

#include "KLSMLocalityTaskStorageItem.h"

#include <pheet/memory/BlockItemReuse/BlockItemReuseMemoryManager.h>

namespace pheet {

template <class Pheet, class TaskStorage, class ParentTaskStoragePlace, class Strategy>
class KLSMLocalityTaskStoragePlace : public ParentTaskStoragePlace::BaseTaskStoragePlace {
public:
	typedef KLSMLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy> Self;

	typedef typename ParentTaskStoragePlace::BaseTaskStoragePlace BaseTaskStoragePlace;
	typedef typename ParentTaskStoragePlace::BaseItem BaseItem;
	typedef KLSMLocalityTaskStorageItem<Pheet, Self, void, BaseItem, Strategy> Item;
	typedef typename BaseItem::T T;

	typedef BlockItemReuseMemoryManager<Pheet, Item, KLSMLocalityTaskStorageItemReuseCheck<Item> > ItemMemoryManager;

	KLSMLocalityTaskStoragePlace(ParentTaskStoragePlace* parent_place)
	:parent_place(parent_place) {

		// Get central task storage at end of initialization (not before,
		// since this place becomes visible as soon as we retrieve it)
		task_storage = TaskStorage::get(this, parent_place->get_central_task_storage(), created_task_storage);
	}
	~KLSMLocalityTaskStoragePlace() {
		if(created_task_storage) {
			delete task_storage;
		}
	}

	void push(Strategy&& strategy, T data) {
		Item& it = items.acquire_item();

		it.strategy = std::move(strategy);
		it.data = data;
		it.taken.store(false, std::memory_order_relaxed);
		it.task_storage = task_storage;

		parent_place->push(&it);
	}

	T pop(BaseItem* boundary) {
		Item* item = reinterpret_cast<Item*>(boundary);
		T data = item->data;
		bool taken = false;
		if(item->taken.compare_exchange_strong(taken, true, std::memory_order_relaxed, std::memory_order_relaxed)) {
			return data;
		}
		return nullable_traits<T>::null_value;
	}

	T steal(BaseItem* boundary) {
		Item* item = reinterpret_cast<Item*>(boundary);
		T data = item->data;
		bool taken = false;
		if(item->taken.compare_exchange_strong(taken, true, std::memory_order_relaxed, std::memory_order_relaxed)) {
			return data;
		}
		return nullable_traits<T>::null_value;
	}

	/*
	 * Override from parent
	 */
	void clean_up() {

	}

private:
	ParentTaskStoragePlace* parent_place;
	TaskStorage* task_storage;
	bool created_task_storage;

	ItemMemoryManager items;

	Item* top;

	size_t delay_phase;
	size_t missed_tasks;
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEPLACE_H_ */
