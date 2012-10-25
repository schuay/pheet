/*
 * CentralKStrategyTaskStoragePlace.h
 *
 *  Created on: 24.10.2012
 *      Author: mwimmer
 */

#ifndef CENTRALKSTRATEGYTASKSTORAGEPLACE_H_
#define CENTRALKSTRATEGYTASKSTORAGEPLACE_H_

#include "CentralKStrategyTaskStorageItem.h"

#include <pheet/memory/ItemReuse/ItemReuseMemoryManager.h>

namespace pheet {

template <class Pheet, class Item>
struct CentralKStrategyTaskStorageItemReference {
	Item* item;
	size_t position;
	// If strategy equals item->strategy => locally created
	typename Pheet::Scheduler::BaseStrategy* strategy;
};

template <class Pheet, class Ref>
class CentralKStrategyTaskStorageStrategyRetriever {
public:
	typedef BasicLinkedListStrategyTaskStorageStrategyRetriever<Pheet, Ref> Self;

	CentralKStrategyTaskStorageStrategyRetriever() {}

	typename Pheet::Scheduler::BaseStrategy* operator()(Ref const& ref) {
		return ref.strategy;
	}

	inline bool is_active(Ref const& ref) {
		return ref.item.position == ref.position;
	}

	inline void mark_removed(Item& item) {
	//	item.block->mark_removed(item.index, task_storage->get_current_view());
	}

private:
};

template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT>
class CentralKStrategyTaskStoragePlace {
public:
	typedef CentralKStrategyTaskStoragePlace<Pheet, TT, StrategyHeapT> Self;

	typedef TT T;
	typedef CentralKStrategyTaskStorageItem<Pheet, TT> Item;
	typedef CentralKStrategyTaskStorageItemReference<Pheet, Item> Ref;
	typedef CentralKStrategyTaskStorageStrategyRetriever<Pheet, Ref> StrategyRetriever;

	typedef StrategyHeapT<Pheet, Ref, StrategyRetriever> StrategyHeap;

	typedef ItemReuseMemoryManager<Pheet, Item, BasicLinkedListStrategyTaskStorageDataBlockReuseCheck<DataBlock> > DataBlockMemoryManager;

	CentralKStrategyTaskStoragePlace();
	~CentralKStrategyTaskStoragePlace();

	template <class Strategy>
	void push(Strategy&& s, T data) {
		Item* it = new Item();
		it->strategy = new Strategy(std::move(s));
		it->data = data;
		// TODO: put to a block and set position and block
		it->position = 0;
		it->block = nullptr;
		it->item_push = &Self::template item_push<Strategy>;

		Ref r;
		r.item = it;
		r.position = it->position;
		r.strategy = it->strategy;

		heap.template push<Strategy>(std::move(r));
	}
	T pop() {
		while(heap.size() > 0) {
			Ref r = heap.pop();

		/*	T ret;
			if(r.block->local_take(r.index, ret, this)) {
				update_front();
				return ret;
			}*/
			return r.item->data;
		}
		return nullable_traits<T>::null_value;
	}

	template <class Strategy>
	void item_push(Item* item) {
		Ref r;
		r.item = item;
		r.position = item.position;
		// TODO: check whether the position is still valid, otherwise end function

		Strategy* s = new Strategy(*reinterpret_cast<Strategy*>(strategy));
		s->rebase();
		r.strategy = s;

		heap.template push<Strategy>(std::move(r));
	}
private:
	StrategyHeap heap;
};

} /* namespace pheet */
#endif /* CENTRALKSTRATEGYTASKSTORAGEPLACE_H_ */
