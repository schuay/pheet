/*
 * LinkedListStrategyTaskStorageView.h
 *
 *  Created on: 07.04.2012
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef LINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_
#define LINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_

#include <vector>
#include "../../../misc/atomics.h"

#include "LinkedListStrategyTaskStorageDataBlock.h"

namespace pheet {

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
class LinkedListStrategyTaskStorageView {
public:
	typedef LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize> Self;
	typedef LinkedListStrategyTaskStorageDataBlock<Pheet, TT, Self, BlockSize> DataBlock;
	typedef MergeablePriorityQueue<Pheet, DataBlock*, typename DataBlock::AgeComparator> FreedBlocksQueue;

	LinkedListStrategyTaskStorageView();
//	LinkedListStrategyTaskStorageView(Self* prev);
	~LinkedListStrategyTaskStorageView();

	DataBlock* get_front() {
		return front;
	}
	size_t get_id() {
		return id;
	}

	void mark_empty(DataBlock* block);

//	void clean(std::vector<Self*>& view_reuse);

	bool try_register();
	void deregister();

	bool needs_cleanup() { return active_blocks < freed_blocks.size(); }

	bool try_reuse();

	void init_first(DataBlock* front);
	void reset(Self* prev);
	void update_front();
private:
	void free_blocks();

	size_t id;

	DataBlock* front;
	Self* prev;
	Self* next;

	size_t active_blocks;
	FreedBlocksQueue freed_blocks;

	ptrdiff_t reg;
};

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::LinkedListStrategyTaskStorageView()
:id(0), front(nullptr), prev(nullptr), next(nullptr), active_blocks(0), reg(-1) {

}
/*
template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::LinkedListStrategyTaskStorageView(DataBlock*& front, Self* prev)
:id(prev->id + 1), front(nullptr), prev(prev), next(nullptr), active_blocks(prev->active_blocks), reg(0) {
	pheet_assert(prev->next == nullptr);
	prev->next = this;
}*/

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::~LinkedListStrategyTaskStorageView() {
	pheet_assert(reg <= 0);

	while(!freed_blocks.empty()) {
		DataBlock* block = freed_blocks.pop();
		delete block;
	}
}

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
void LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::mark_empty(DataBlock* block) {
	--active_blocks;
	freed_blocks.push(block);
	if(front == block) {
		block = front->get_next();
	}
}

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
bool LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::try_register() {
	ptrdiff_t r = reg;
	if(r >= 0) {
		if(PTRDIFFT_CAS(&reg, r, r + 1)) {
			return true;
		}
	}
	return false;
}

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
void LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::deregister() {
	pheet_assert(reg > 0);
	PTRDIFFT_ATOMIC_SUB(&reg, 1);
}

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
bool LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::try_reuse() {
	if(reg == 0) {
		if(next == nullptr) {
			// View is still active. can't be reused
			return false;
		}
		if(PTRDIFFT_CAS(&reg, 0, -1)) {
			free_blocks();
			return true;
		}
	}
	return reg == -1;
}

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
void LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::free_blocks() {
	pheet_assert(next != nullptr);
	if(prev == nullptr) {
		while(!freed_blocks.empty()) {
			DataBlock* db = freed_blocks.pop();
			delete db;
		}
		next->prev = nullptr;
	}
	else {
		pheet_assert(((ptrdiff_t)(id - prev->id)) > 0);
		size_t prev_id = prev->id;

		while(!freed_blocks.empty() && ((ptrdiff_t)(freed_blocks.peek()->get_first_view_id() - prev_id)) > 0) {
			DataBlock* db = freed_blocks.pop();
			delete db;
		}

		prev->freed_blocks.merge(std::move(freed_blocks));
		prev->next = next;
		next->prev = prev;
	}
	pheet_assert(freed_blocks.empty());
}
/*
template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
void LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::clean(std::vector<Self*>& view_reuse) {
	pheet_assert(reg == -1);
	for(auto i = freed_blocks.begin(); i != freed_blocks.end(); ++i) {
		if((*i)->get_first_view() == this) {
			--active_blocks;
			delete *i;
		}
		else {
			pheet_assert(prev != nullptr);
			prev->freed_blocks.push_back(*i);
		}
	}
	freed_blocks.clear();
	if(prev != nullptr && prev->reg == -1) {
		prev->clean(view_reuse);
		if(active_blocks == 0) {
			prev->next = next;
		}
	}
	pheet_assert(next != nullptr);
	if(active_blocks == 0) {
		next->prev = prev;
		view_reuse.push_back(this);
	}
}*/


template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
void LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::init_first(DataBlock* front) {
	pheet_assert(reg == -1);
	pheet_assert(prev == nullptr);
	pheet_assert(id == 0);
	pheet_assert(active_blocks == 0);
	pheet_assert(freed_blocks.empty());

	this->front = front;
//	this->prev = prev;
//	this->id = prev->id + 1;
	next = nullptr;
	active_blocks = 1;

	MEMORY_FENCE();

	reg = 0;
}

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
void LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::reset(Self* prev) {
	pheet_assert(reg == -1);
//	pheet_assert(prev != nullptr);
	pheet_assert(freed_blocks.empty());

	this->front = prev->front;
	this->prev = prev;
	this->id = prev->id + 1;
	next = nullptr;
	active_blocks = prev->active_blocks;
	pheet_assert(prev->next == nullptr);
	prev->next = this;

	MEMORY_FENCE();

	reg = 0;
}

template <class Pheet, typename TT, template <class, typename, class> class MergeablePriorityQueue, size_t BlockSize>
void LinkedListStrategyTaskStorageView<Pheet, TT, MergeablePriorityQueue, BlockSize>::update_front() {
	while(!front->is_active()) {
		front = front->get_next();
	}
}


}

#endif /* LINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_ */
