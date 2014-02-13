/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef PARETOLOCALITYTASKSTORAGEPLACE_H_
#define PARETOLOCALITYTASKSTORAGEPLACE_H_

#include "ParetoLocalityTaskStorageBlock.h"
#include "ParetoLocalityTaskStorageItem.h"
#include "ParetoLocalityTaskStorageItemReuseCheck.h"

#include "pheet/memory/ItemReuse/ItemReuseMemoryManager.h"
#include "pheet/misc/type_traits.h"


#include <vector>

#define MAX_PARTITION_SIZE (4)

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
	typedef ParetoLocalityTaskStorageBlock<Item, MAX_PARTITION_SIZE> Block;
	typedef typename BaseItem::T T;
	typedef ItemReuseMemoryManager<Pheet, Item, ParetoLocalityTaskStorageItemReuseCheck<Item>>
	        ItemMemoryManager;


	ParetoLocalityTaskStoragePlace(ParentTaskStoragePlace* parent_place);
	~ParetoLocalityTaskStoragePlace();

	void push(Strategy&& strategy, T data);
	T pop(BaseItem* boundary);
	T steal(BaseItem* boundary);
	void clean_up();

private:
	/**
	 * A merge is required if:
	 * - last->prev() exists
	 * - and has the same level (equal to same capacity) as last
	 */
	bool merge_required() const;

private:
	ParentTaskStoragePlace* parent_place;
	TaskStorage* task_storage;
	bool created_task_storage;

	ItemMemoryManager items;

	VirtualArray<Item*> m_array;
	PivotQueue m_pivots;
	Block* first;
	Block* last;
};

template < class Pheet,
           class TaskStorage,
           class ParentTaskStoragePlace,
           class Strategy >
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
ParetoLocalityTaskStoragePlace(ParentTaskStoragePlace* parent_place)
	: parent_place(parent_place)
{

	first = new Block(m_array, 0, &m_pivots);
	last = first;
	m_array.increase_capacity(first->capacity());
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

	while (last->prev()) {
		last = last->prev();
		delete last->next();
	}
	delete last;
}

template < class Pheet,
           class TaskStorage,
           class ParentTaskStoragePlace,
           class Strategy >
void
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
push(Strategy&& strategy, T data)
{
	Item& item = items.acquire_item();
	item.data = data;
	item.strategy(std::forward < Strategy && > (strategy));
	item.task_storage = task_storage;
	// Release the item so that other threads can see it.
	item.taken.store(false, std::memory_order_release);

	if (!last->try_put(&item)) {
		//merge if neccessary
		if (merge_required()) {
			//merge recursively, if previous block has same level
			while (merge_required()) {
				last = last->prev()->merge_next();
			}
			//repartition block that resulted from merge
			last->partition();
		}

		//create new block
		size_t nb_offset = last->offset() + last->capacity();
		Block* nb = new Block(m_array, nb_offset, &m_pivots);
		nb->prev(last);
		pheet_assert(!last->next());
		last->next(nb);
		last = nb;
		m_array.increase_capacity(nb->capacity());
		//put the item in the new block
		last->put(&item);
	}

	parent_place->push(&item);
}

template < class Pheet,
           class TaskStorage,
           class ParentTaskStoragePlace,
           class Strategy >
typename ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::T
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
pop(BaseItem* /* boundary */)
{
	//iterate through all blocks
	Block* best;
	Item* item = nullptr;

	for (Block* it = first; it != nullptr; it = it->next()) {
		Item* const top = it->top();
		//is the block empty?
		if (top == nullptr) {
			/* TODO: it->top() returned nullptr, thus no more active items are in
			 * block it. The underlying VirtualArray can be reused/fred from
			 * m_data[it->m_offset] to m_data[m_offset + it->m_capacity] */
			continue;
		}

		if (item == nullptr || top->strategy()->prioritize(*(item->strategy()))) {
			item = top;
			best = it;
		}
	}

	if (item == nullptr) {
		return nullable_traits<T>::null_value;
	}
	return best->take(item);
}

template < class Pheet,
           class TaskStorage,
           class ParentTaskStoragePlace,
           class Strategy >
typename ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::T
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
steal(BaseItem* /* boundary */)
{
	//TODO: use spy semantics
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

template < class Pheet,
           class TaskStorage,
           class ParentTaskStoragePlace,
           class Strategy >
bool
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
merge_required() const
{
	return last->prev() && last->lvl() == last->prev()->lvl();
}

} /* namespace pheet */

#endif /* PARETOLOCALITYTASKSTORAGEPLACE_H_ */
