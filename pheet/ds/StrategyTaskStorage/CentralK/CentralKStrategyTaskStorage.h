/*
 * CentralKStrategyTaskStorage.h
 *
 *  Created on: 24.10.2012
 *      Author: mwimmer
 */

#ifndef CENTRALKSTRATEGYTASKSTORAGE_H_
#define CENTRALKSTRATEGYTASKSTORAGE_H_

namespace pheet {



template <class Pheet, typename TT, template <class SP, typename ST, class SR> class StrategyHeapT, size_t BlockSize, size_t BlockPadding, size_t BlockReuse, size_t MaxK>
class CentralKStrategyTaskStorageImpl {
public:
	typedef CentralKStrategyTaskStorageImpl<Pheet, TT, StrategyHeap, BlockSize, BlockPadding, BlockReuse, MaxK> Self;

	typedef TT T;

	CentralKStrategyTaskStorageImpl();
	~CentralKStrategyTaskStorageImpl();
};

template <class Pheet, typename TT>
using CentralKStrategyTaskStorage<Pheet, TT, BasicStrategyHeap, 256, 1, 1, 256>;

} /* namespace pheet */
#endif /* CENTRALKSTRATEGYTASKSTORAGE_H_ */
