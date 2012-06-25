/*
 * BasicLinkedListStrategyTaskStorageStream.h
 *
 *  Created on: Apr 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef BASICLINKEDLISTSTRATEGYTASKSTORAGESTREAM_H_
#define BASICLINKEDLISTSTRATEGYTASKSTORAGESTREAM_H_

namespace pheet {

template <class Pheet, class StreamRef>
class BasicLinkedListStrategyTaskStorageStreamRefStrategyRetriever {
public:
	typename StreamRef::BaseStrategy* operator()(StreamRef& sr) {
		return sr.get_strategy();
	}
};

template <class Pheet, class DataBlock>
class BasicLinkedListStrategyTaskStorageStreamRef {
public:
	typedef BasicLinkedListStrategyTaskStorageStreamRef<Pheet, DataBlock> Self;
	typedef typename DataBlock::Item Item;
	typedef typename Pheet::Scheduler::BaseStrategy BaseStrategy;
	typedef BasicLinkedListStrategyTaskStorageStreamRefStrategyRetriever<Pheet, Self> StrategyRetriever;

	BasicLinkedListStrategyTaskStorageStreamRef() {
	}

	BasicLinkedListStrategyTaskStorageStreamRef(DataBlock* block, int index)
	:block(block), index(index), taken_offset(block->get_taken_offset()) {}

	bool take(Item& item) {
		return block->take(index, taken_offset, item);
	}

	BaseStrategy* get_strategy() {
		return block->get_strategy(index, taken_offset);
	}

	DataBlock* block;
	int index;
	size_t taken_offset;
};

template <class Pheet, class TaskStorage, class StealerRef>
class BasicLinkedListStrategyTaskStorageStream {
public:
	typedef typename TaskStorage::Item Item;
	typedef typename TaskStorage::View View;
	typedef typename TaskStorage::DataBlock DataBlock;
	typedef BasicLinkedListStrategyTaskStorageStreamRef<Pheet, DataBlock> StreamRef;

	BasicLinkedListStrategyTaskStorageStream(TaskStorage& task_storage);
	~BasicLinkedListStrategyTaskStorageStream();

	inline StreamRef get_ref() {
		return StreamRef(block, block_index);
	}

	bool has_next() {
		int bin = block_index + 1;
		return bin < block->get_size() || (bin == block->get_max_size() && block->get_next() != nullptr && block->get_next()->get_size() > 0);
	/*	if(!hn && old_view != nullptr) {
			// We can now safely deregister from the old view
			old_view->deregister();
		}
		return hn;*/
	}

	void next() {
		pheet_assert(has_next());
		check_current_view();

		// Might lead to problems under some relaxed memory consistency models,
		// since a new size may be available even though the value in the array is still in some cache
		// TODO: Fix this with C++11 memory model
		++block_index;
		if(block_index == block->get_max_size()) {
			block = block->get_next();
			block_index = 0;
		}
		pheet_assert(block_index < block->get_max_size());
		pheet_assert(block != nullptr);
	}

	void stealer_push_ref(StealerRef& stealer) {
		auto sp = block->get_data(block_index).stealer_push;
		(this->*sp)(stealer);
	}

	template <class Strategy>
	void stealer_push_ref(StealerRef& stealer) {
		stealer.template push<Strategy>(get_ref());
	}
private:
	void check_current_view();

	TaskStorage& task_storage;

	View* current_view;
	View* old_view;
	DataBlock* block;
	int block_index;
};

template <class Pheet, class TaskStorage, class StealerRef>
BasicLinkedListStrategyTaskStorageStream<Pheet, TaskStorage, StealerRef>::BasicLinkedListStrategyTaskStorageStream(TaskStorage& task_storage)
: task_storage(task_storage),
  current_view(task_storage.acquire_current_view()),
  old_view(nullptr),
  block(current_view->get_front()),
  block_index(-1) {

}

template <class Pheet, class TaskStorage, class StealerRef>
BasicLinkedListStrategyTaskStorageStream<Pheet, TaskStorage, StealerRef>::~BasicLinkedListStrategyTaskStorageStream() {
	// TODO: make sure there are no deadlocks/leaks if we don't deregister!
/*	if(old_view != nullptr)
		old_view->deregister();
	current_view->deregister();*/
}

template <class Pheet, class TaskStorage, class StealerRef>
inline void
BasicLinkedListStrategyTaskStorageStream<Pheet, TaskStorage, StealerRef>::check_current_view() {
	if(task_storage.get_current_view() != current_view) {
		if(old_view == nullptr) {
			old_view = current_view;
		}
		else {
			current_view->deregister();
		}
		current_view = task_storage.acquire_current_view();
	}
	if(old_view != nullptr && block->get_next() == nullptr) {
		old_view->deregister();
		old_view = nullptr;
	}
}

}

#endif /* BASICLINKEDLISTSTRATEGYTASKSTORAGESTREAM_H_ */
