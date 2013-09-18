/*
 * Strategy2BaseTaskStorageItem.h
 *
 *  Created on: Aug 12, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef STRATEGY2BASETASKSTORAGEITEM_H_
#define STRATEGY2BASETASKSTORAGEITEM_H_


namespace pheet {

template <class Pheet, class TaskStorage, typename TT>
struct Strategy2BaseTaskStorageBaseItem {
//	typedef StrategyScheduler2TaskStorageBase<Pheet, TT> TaskStorage;
//	typedef StrategyScheduler2BaseStrategy<Pheet> BaseStrategy;
	typedef TT T;

	Strategy2BaseTaskStorageBaseItem()
	:taken(true) {}
	~Strategy2BaseTaskStorageBaseItem() {}

	TT data;
	std::atomic<bool> taken;
	TaskStorage* task_storage;
//	BaseStrategy* strategy;
};

template <class Pheet, class TaskStorage, class DataBlock, typename TT>
struct Strategy2BaseTaskStorageItem : Strategy2BaseTaskStorageBaseItem<Pheet, TaskStorage, TT> {
	typedef StrategyScheduler2BaseStrategy<Pheet> BaseStrategy;

	Strategy2BaseTaskStorageItem() {
//		strategy = &strategy_v;
	}
	~Strategy2BaseTaskStorageItem() {}

//	DataBlock* data_block;
//	size_t offset;
};

template <class Item>
struct Strategy2BaseTaskStorageItemReuseCheck {
	bool operator() (Item const& item) const {
		return item.taken;
	}
};

} /* namespace pheet */
#endif /* STRATEGY2BASETASKSTORAGEITEM_H_ */
