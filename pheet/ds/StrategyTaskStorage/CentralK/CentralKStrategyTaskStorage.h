/*
 * CentralKStrategyTaskStorage.h
 *
 *  Created on: 24.10.2012
 *      Author: mwimmer
 */

#ifndef CENTRALKSTRATEGYTASKSTORAGE_H_
#define CENTRALKSTRATEGYTASKSTORAGE_H_

#include "CentralKStrategyTaskStoragePlace.h"
#include "CentralKStrategyTaskStorageDataBlock.h"

namespace pheet {



template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize, size_t Tests, bool LocalKPrio>
class CentralKStrategyTaskStorageImpl {
public:
	typedef CentralKStrategyTaskStorageImpl<Pheet, TT, StrategyHeapT, BlockSize, Tests, LocalKPrio> Self;

	typedef CentralKStrategyTaskStoragePlace<Pheet, Self, TT, StrategyHeapT, BlockSize, Tests, LocalKPrio> Place;
	typedef CentralKStrategyTaskStorageDataBlock<Pheet, Place, TT, BlockSize, Tests> DataBlock;

	typedef TT T;

	CentralKStrategyTaskStorageImpl(size_t num_places)
	:head(0), tail(0), start_block(nullptr), num_places(num_places) {}
	~CentralKStrategyTaskStorageImpl() {}

	size_t get_num_places() {
		return num_places;
	}

	size_t head;
	size_t tail;

	DataBlock* start_block;

	static void print_name() {
		std::cout << "CentralKStrategyTaskStorage";
	}

private:
	size_t num_places;
};

template <class Pheet, typename TT>
using CentralKStrategyTaskStorage = CentralKStrategyTaskStorageImpl<Pheet, TT, BasicStrategyHeap, 512, 128, true>;

} /* namespace pheet */
#endif /* CENTRALKSTRATEGYTASKSTORAGE_H_ */
