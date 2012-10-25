/*
 * CentralKStrategyTaskStorageDataBlock.h
 *
 *  Created on: 25.10.2012
 *      Author: mwimmer
 */

#ifndef CENTRALKSTRATEGYTASKSTORAGEDATABLOCK_H_
#define CENTRALKSTRATEGYTASKSTORAGEDATABLOCK_H_

#include "CentralKStrategyTaskStorageItem.h"

namespace pheet {

template <class Pheet, typename TT>
class CentralKStrategyTaskStorageDataBlock {
public:
	typedef CentralKStrategyTaskStorageDataBlock<Pheet, TT> Self;

	typedef CentralKStrategyTaskStorageItem<Pheet, TT> Item;

	CentralKStrategyTaskStorageDataBlock();
	~CentralKStrategyTaskStorageDataBlock();

	void put(Item* item);
	void verify(Item* item, size_t position);
	bool take(Item* item, size_t position);
};

} /* namespace pheet */
#endif /* CENTRALKSTRATEGYTASKSTORAGEDATABLOCK_H_ */
