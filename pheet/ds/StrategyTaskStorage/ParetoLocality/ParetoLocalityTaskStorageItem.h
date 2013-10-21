/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef PARETOLOCALITYTASKSTORAGEITEM_H_
#define PARETOLOCALITYTASKSTORAGEITEM_H_

namespace pheet
{

template < class Pheet,
         class Place,
         class BaseItem,
         class Strategy >
struct ParetoLocalityTaskStorageItem : public BaseItem
{
	typedef typename BaseItem::T T;

	ParetoLocalityTaskStorageItem();
	T take();
	void take_and_delete();


	Place* owner;
	Strategy strategy;
};

template < class Pheet,
         class Place,
         class BaseItem,
         class Strategy >
ParetoLocalityTaskStorageItem<Pheet, Place, BaseItem, Strategy>::
ParetoLocalityTaskStorageItem()
	: owner(nullptr)
{

}

template < class Pheet,
         class Place,
         class BaseItem,
         class Strategy >
typename ParetoLocalityTaskStorageItem<Pheet, Place, BaseItem, Strategy>::T
ParetoLocalityTaskStorageItem<Pheet, Place, BaseItem, Strategy>::
take()
{
	//TODO: no concurrency yet
	this->taken.store(true, std::memory_order_relaxed);
	return this->data;
}

template < class Pheet,
         class Place,
         class BaseItem,
         class Strategy >
void
ParetoLocalityTaskStorageItem<Pheet, Place, BaseItem, Strategy>::
take_and_delete()
{
	//TODO: no concurrency yet
	this->taken.store(true, std::memory_order_relaxed);
	this->data.drop_item();
}

} /* namespace pheet */
#endif /* PARETOLOCALITYTASKSTORAGEITEM_H_ */

