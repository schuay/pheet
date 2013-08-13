/*
 * Strategy2BaseTaskStorage.h
 *
 *  Created on: 12.08.2013
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef STRATEGY2BASETASKSTORAGE_H_
#define STRATEGY2BASETASKSTORAGE_H_

#include "Strategy2BaseTaskStoragePlace.h"
#include "Strategy2BaseTaskStorageDataBlock.h"
#include <pheet/ds/StrategyHeap/Basic/BasicStrategyHeap.h>

namespace pheet {


/**
 * Base task storage with arora-like behaviour (difference is that spying is used instead of stealing)
 * Properties: Only place in strategy is used.
 */
template <class Pheet, typename TT, size_t BlockSize>
class Strategy2BaseTaskStorageImpl {
public:
	typedef Strategy2BaseTaskStorageImpl<Pheet, TT, BlockSize> Self;

	typedef Strategy2BaseTaskStoragePlace<Pheet, Self, TT, BlockSize> Place;
	typedef Strategy2BaseTaskStorageDataBlock<Pheet, Place, TT, BlockSize> DataBlock;

	typedef TT T;

	template <size_t NewBlockSize>
	using WithBlockSize = Strategy2BaseTaskStorageImpl<Pheet, TT, NewBlockSize>;

	Strategy2BaseTaskStorageImpl(size_t num_places)
	:num_places(num_places) {}
	~Strategy2BaseTaskStorageImpl() {}

	static void print_name() {
		std::cout << "Strategy2BaseTaskStorage<" << BlockSize << ">";
	}

private:
};

template <class Pheet, typename TT>
using Strategy2BaseTaskStorage = Strategy2BaseTaskStorageImpl<Pheet, TT, BasicStrategyHeap, 128, false>;

template <class Pheet, typename TT>
using Strategy2BaseTaskStorageLocalK = Strategy2BaseTaskStorageImpl<Pheet, TT, BasicStrategyHeap, 128, true>;

} /* namespace pheet */
#endif /* STRATEGY2BASETASKSTORAGE_H_ */
