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
	return this->data;
}

} /* namespace pheet */
#endif /* PARETOLOCALITYTASKSTORAGEITEM_H_ */

