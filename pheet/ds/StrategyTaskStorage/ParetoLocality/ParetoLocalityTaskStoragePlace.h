/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef PARETOLOCALITYTASKSTORAGEPLACE_H_
#define PARETOLOCALITYTASKSTORAGEPLACE_H_

#include "ParetoLocalityTaskStorageItem.h"

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

	ParetoLocalityTaskStoragePlace(ParentTaskStoragePlace* parent);
	~ParetoLocalityTaskStoragePlace();

	void push(Strategy&& strategy, T data);
	T pop(BaseItem* boundary);
	T steal(BaseItem* boundary);
	void clean_up();

};

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
ParetoLocalityTaskStoragePlace(ParentTaskStoragePlace* parent)
{

}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
~ParetoLocalityTaskStoragePlace()
{

}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
void
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
push(Strategy&& strategy, T data)
{

}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
typename ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::T
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
pop(BaseItem* boundary)
{

}

template < class Pheet,
         class TaskStorage,
         class ParentTaskStoragePlace,
         class Strategy >
typename ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::T
ParetoLocalityTaskStoragePlace<Pheet, TaskStorage, ParentTaskStoragePlace, Strategy>::
steal(BaseItem* boundary)
{

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
