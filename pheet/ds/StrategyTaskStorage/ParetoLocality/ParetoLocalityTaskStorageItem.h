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
         class Place,   //TODO: do we need the place that owns the item here?
         class BaseItem,
         class Strategy >
class ParetoLocalityTaskStorageItem : public BaseItem
{
public:
	typedef typename BaseItem::T T;

	ParetoLocalityTaskStorageItem(Strategy&& strategy, T data);
	T take();
	void take_and_delete();
	bool is_taken();
	bool is_taken_or_dead();
	Strategy* strategy();

private:
	Strategy m_strategy;
};

template < class Pheet,
         class Place,
         class BaseItem,
         class Strategy >
ParetoLocalityTaskStorageItem<Pheet, Place, BaseItem, Strategy>::
ParetoLocalityTaskStorageItem(Strategy&& strategy, T t)
{
	m_strategy = std::move(strategy);
	this->data = t;
	this->taken.store(false, std::memory_order_release);
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

template < class Pheet,
         class Place,
         class BaseItem,
         class Strategy >
bool
ParetoLocalityTaskStorageItem<Pheet, Place, BaseItem, Strategy>::
is_taken()
{
	return this->taken.load();
}

template < class Pheet,
         class Place,
         class BaseItem,
         class Strategy >
bool
ParetoLocalityTaskStorageItem<Pheet, Place, BaseItem, Strategy>::
is_taken_or_dead()
{
	return m_strategy.dead_task() || this->taken.load();
}

template < class Pheet,
         class Place,
         class BaseItem,
         class Strategy >
Strategy*
ParetoLocalityTaskStorageItem<Pheet, Place, BaseItem, Strategy>::
strategy()
{
	return &m_strategy;
}


} /* namespace pheet */
#endif /* PARETOLOCALITYTASKSTORAGEITEM_H_ */

