/*
 * BasicLinkedListStrategyTaskStorageView.h
 *
 *  Created on: 07.04.2012
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef BASICLINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_
#define BASICLINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_

#include <vector>
#include "../../../misc/atomics.h"

#include "BasicLinkedListStrategyTaskStorageDataBlock.h"

namespace pheet {

template <class Pheet, typename TT, size_t BlockSize>
class BasicLinkedListStrategyTaskStorageView {
public:
	typedef BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize> Self;
	typedef BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, Self, BlockSize> DataBlock;

	BasicLinkedListStrategyTaskStorageView();
//	BasicLinkedListStrategyTaskStorageView(Self* prev);
	~BasicLinkedListStrategyTaskStorageView();

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

	bool needs_cleanup() { return active_blocks < num_freed_blocks; }

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
	DataBlock* first_freed_block;
	DataBlock* last_freed_block;
	size_t num_freed_blocks;

	ptrdiff_t reg;
};

template <class Pheet, typename TT, size_t BlockSize>
BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::BasicLinkedListStrategyTaskStorageView()
:id(0), front(nullptr), prev(nullptr), next(nullptr), active_blocks(0), first_freed_block(nullptr), last_freed_block(nullptr), num_freed_blocks(0), reg(-1) {

}
/*
template <class Pheet, typename TT, size_t BlockSize>
BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::BasicLinkedListStrategyTaskStorageView(DataBlock*& front, Self* prev)
:id(prev->id + 1), front(nullptr), prev(prev), next(nullptr), active_blocks(prev->active_blocks), reg(0) {
	pheet_assert(prev->next == nullptr);
	prev->next = this;
}*/

template <class Pheet, typename TT, size_t BlockSize>
BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::~BasicLinkedListStrategyTaskStorageView() {
//	pheet_assert(reg <= 0);

	DataBlock* cur = first_freed_block;
	DataBlock* nextb;
	while(cur != nullptr) {
		nextb = cur->get_next_freed();
		delete cur;
		cur = nextb;
	}
}

template <class Pheet, typename TT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::mark_empty(DataBlock* block) {
	--active_blocks;
	++num_freed_blocks;
	if(first_freed_block != nullptr) {
		last_freed_block->set_next_freed(block);
		last_freed_block = block;
	}
	else {
		first_freed_block = block;
		last_freed_block = block;
	}
	if(front == block) {
		block = front->get_next();
	}
}

template <class Pheet, typename TT, size_t BlockSize>
bool BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::try_register() {
	ptrdiff_t r = reg;
	if(r >= 0) {
		if(PTRDIFFT_CAS(&reg, r, r + 1)) {
			return true;
		}
	}
	return false;
}

template <class Pheet, typename TT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::deregister() {
	pheet_assert(reg > 0);
	PTRDIFFT_ATOMIC_SUB(&reg, 1);
}

template <class Pheet, typename TT, size_t BlockSize>
bool BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::try_reuse() {
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

template <class Pheet, typename TT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::free_blocks() {
	pheet_assert(next != nullptr);
	if(prev == nullptr) {
		DataBlock* cur = first_freed_block;
		DataBlock* nextb;
		while(cur != nullptr) {
			nextb = cur->get_next_freed();
			delete cur;
			cur = nextb;
		}
		next->prev = nullptr;
	}
	else {
		pheet_assert(prev->first_freed_block != nullptr);
		prev->last_freed_block->set_next_freed(first_freed_block);
		prev->last_freed_block = last_freed_block;
		prev->num_freed_blocks += num_freed_blocks;

		prev->next = next;
		next->prev = prev;
	}
	first_freed_block = nullptr;
	last_freed_block = nullptr;
	num_freed_blocks = 0;
}
/*
template <class Pheet, typename TT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::clean(std::vector<Self*>& view_reuse) {
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


template <class Pheet, typename TT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::init_first(DataBlock* front) {
	pheet_assert(reg == -1);
	pheet_assert(prev == nullptr);
	pheet_assert(id == 0);
	pheet_assert(active_blocks == 0);
	pheet_assert(first_freed_block == nullptr);

	this->front = front;
//	this->prev = prev;
//	this->id = prev->id + 1;
	next = nullptr;
	active_blocks = 1;

	MEMORY_FENCE();

	reg = 0;
}

template <class Pheet, typename TT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::reset(Self* prev) {
	pheet_assert(reg == -1);
//	pheet_assert(prev != nullptr);
	pheet_assert(first_freed_block == nullptr);

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

template <class Pheet, typename TT, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageView<Pheet, TT, BlockSize>::update_front() {
	while(!front->is_active()) {
		front = front->get_next();
	}
}


}

#endif /* BASICLINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_ */
