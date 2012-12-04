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



template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize, size_t Tests>
class CentralKStrategyTaskStorageImpl {
public:
	typedef CentralKStrategyTaskStorageImpl<Pheet, TT, StrategyHeap, BlockSize, Tests> Self;

	typedef CentralKStrategyTaskStoragePlace<Pheet, TT, StrategyHeapT, BlockSize, Tests> Place;
	typedef CentralKStrategyTaskStorageDataBlock<Pheet, Place, TT, BlockSize, Tests> DataBlock;

	typedef TT T;

	CentralKStrategyTaskStorageImpl();
	~CentralKStrategyTaskStorageImpl();

private:
	size_t head;
	size_t tail;
};

template <class Pheet, typename TT>
using CentralKStrategyTaskStorage<Pheet, TT, BasicStrategyHeap, 512, 128>;

} /* namespace pheet */
#endif /* CENTRALKSTRATEGYTASKSTORAGE_H_ */
