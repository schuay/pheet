/*
 * BasicLinkedListStrategyTaskStorageDataBlock.h
 *
 *  Created on: 07.04.2012
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef BASICLINKEDLISTSTRATEGYTASKSTORAGEDATABLOCK_H_
#define BASICLINKEDLISTSTRATEGYTASKSTORAGEDATABLOCK_H_


namespace pheet {

/*
 * less for age. (younger is less)
 */
template <class DataBlock>
struct BasicLinkedListStrategyTaskStorageDataBlockAgeComparator {
	bool operator()(DataBlock* b1, DataBlock* b2) {
		return ((ptrdiff_t)(b1->get_first_view_id() - b2->get_first_view_id())) > 0;
	}
};

template <class Pheet, typename TT, class View, size_t BlockSize>
class BasicLinkedListStrategyTaskStorageDataBlock {
public:
	typedef TT T;
	typedef typename T::Item Item;
	typedef BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize> Self;
	typedef BasicLinkedListStrategyTaskStorageDataBlockAgeComparator<Self> AgeComparator;

	BasicLinkedListStrategyTaskStorageDataBlock(size_t id, Self* prev);
	~BasicLinkedListStrategyTaskStorageDataBlock();

	bool local_take(size_t index, typename T::Item& ret, View* current_view);
	bool take(size_t index, size_t stored_taken_offset, typename T::Item& ret);
	bool take(size_t index, size_t stored_taken_offset);
	void mark_removed(size_t index, View* current_view);
	T& peek(size_t index);

	size_t push(T&& item, View* current_view);

	inline T& get_data(size_t index) {
		pheet_assert(index < filled);
		return data[index];
	}
	inline T const& get_data(size_t index) const {
		pheet_assert(index < filled);
		return data[index];
	}
	inline typename Pheet::Scheduler::BaseStrategy* get_strategy(size_t index, size_t stored_taken_offset) {
		if(data[index].taken != stored_taken_offset) {
			return nullptr;
		}
		return data[index].strategy;
	}
	inline Self* get_next() { return next; }
	inline bool is_active() {
		pheet_assert(active != 0 || next != nullptr);
		return active != 0;
	}
	inline bool is_taken(size_t index, size_t stored_taken_offset) {
		return data[index].taken == stored_taken_offset;
	}
	inline size_t get_size() { return filled; }
	inline size_t get_max_size() { return BlockSize; }

	inline void set_next_freed(Self* nf) {
		pheet_assert(next_freed == nullptr);
		next_freed = nf;
	}
	inline Self* get_next_freed() { return next_freed; }

	inline size_t get_taken_offset() { return taken_offset; }

	void reset_content();
	void reuse(size_t id, Self* prev);

private:
	void clean(View* current_view);

	T data[BlockSize];

	size_t id;
	Self* prev;
	Self* next;
	Self* orig_prev;
	Self* orig_next;
	Self* next_freed;
	size_t filled;

	size_t active;

	size_t taken_offset;
};

template <class Pheet, typename TT, class View, size_t BlockSize>
BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::BasicLinkedListStrategyTaskStorageDataBlock(size_t id, Self* prev)
:id(id), prev(prev), next(nullptr), orig_prev(prev), orig_next(next), next_freed(nullptr), filled(0), active(BlockSize), taken_offset(0) {
	if(orig_prev != nullptr) {
		pheet_assert(orig_prev->orig_next == nullptr);
		orig_prev->orig_next = this;
	}
}

template <class Pheet, typename TT, class View, size_t BlockSize>
BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::~BasicLinkedListStrategyTaskStorageDataBlock() {
	reset_content();
	if(orig_prev != nullptr) {
		orig_prev->orig_next = orig_next;
	}
	if(orig_next != nullptr) {
		orig_next->orig_prev = orig_prev;
	}
}

template <class Pheet, typename TT, class View, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::mark_removed(size_t index, View* current_view) {
	pheet_assert(active > 0);
	pheet_assert(data[index].taken == 1);

	--active;
	clean(current_view);
}

template <class Pheet, typename TT, class View, size_t BlockSize>
bool BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::local_take(size_t index, typename T::Item& ret, View* current_view) {
	pheet_assert(active > 0);
	pheet_assert(index < filled);
	--active;

	if(data[index].taken == taken_offset) {
		if(SIZET_CAS(&(data[index].taken), taken_offset, taken_offset + 1)) {
			ret = data[index].item;
			clean(current_view);
			return true;
		}
	}
	clean(current_view);
	return false;
}

template <class Pheet, typename TT, class View, size_t BlockSize>
bool BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::take(size_t index, size_t stored_taken_offset, typename T::Item& ret) {
	pheet_assert(index < filled);

	if(data[index].taken == stored_taken_offset) {
		if(SIZET_CAS(&(data[index].taken), stored_taken_offset, stored_taken_offset + 1)) {
			ret = data[index].item;
			return true;
		}
	}
	return false;
}

template <class Pheet, typename TT, class View, size_t BlockSize>
bool BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::take(size_t index, size_t stored_taken_offset) {
	pheet_assert(index < filled);

	if(data[index].taken == stored_taken_offset) {
		if(SIZET_CAS(&(data[index].taken), stored_taken_offset, stored_taken_offset + 1)) {
			return true;
		}
	}
	return false;
}
/*
template <class Pheet, typename TT, class View, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::mark_removed(size_t index, View* current_view) {
	pheet_assert(active > 0);
	pheet_assert(index < filled);
	--active;
	clean(current_view);
}*/

template <class Pheet, typename TT, class View, size_t BlockSize>
TT& BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::peek(size_t index) {
	pheet_assert(active > 0);
	pheet_assert(index < filled);

	return data[index].item;
}

template <class Pheet, typename TT, class View, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::clean(View* current_view) {
	// There should be no problems with id wraparound, except that block with id 0 ignores rules
	// This means O(log(n)) is not satisfied in for every predecessor of block 0
	// This leads to a complexity of O((n/(size_t_max + 1))*log(size_t_max)) for n > size_t max which isn't really bad
	// and rare in any case (it's linear to the number of wraparounds and not logarithmic)

	if(active == 0 // Check if block is not used any more
			&& (orig_prev == nullptr // Either no predecessor exists anymore
					// Or make sure the next block is a higher power of two (to guarantee O(log(n)) access times
					// for other threads missing elements (don't want them to have O(n) for elements they never need)
					|| ((next->id & id) != id))) {
		 // Only free blocks if they have a successor (I believe as it may only be inactive if there is a successor)
		pheet_assert(next != nullptr);
		next->prev = prev;
		current_view->mark_empty(this);
		if(prev != nullptr) {
			prev->next = next;
			// Tail recursion
			prev->clean(current_view);
		}
	}
	current_view->update_front();
}

template <class Pheet, typename TT, class View, size_t BlockSize>
size_t BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::push(T&& item, View* current_view) {
	pheet_assert(filled < BlockSize);

	data[filled] = item;

	if(filled == BlockSize - 1) {
		Self* tmp = new Self(id + 1, this);
		MEMORY_FENCE();
		next = tmp;
	}
	else {
		MEMORY_FENCE();
	}
	++filled;
	return filled - 1;
}


template <class Pheet, typename TT, class View, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::reset_content() {
	for(size_t i = 0; i < filled; ++i) {
		delete data[i].strategy;
	}
	filled = 0;
}


template <class Pheet, typename TT, class View, size_t BlockSize>
void BasicLinkedListStrategyTaskStorageDataBlock<Pheet, TT, View, BlockSize>::reuse(size_t id, Self* prev) {
	pheet_assert(filled == 0);

	this->id = id;
	this->prev = prev;
	this->orig_prev = prev;
	next = nullptr;
	orig_next = nullptr;
	active = BlockSize;
	taken_offset += 2;
}



}

#endif /* BASICLINKEDLISTSTRATEGYTASKSTORAGEDATABLOCK_H_ */
