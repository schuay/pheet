/*
 * LinkedListStrategyTaskStorageDataBlock.h
 *
 *  Created on: 07.04.2012
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef LINKEDLISTSTRATEGYTASKSTORAGEDATABLOCK_H_
#define LINKEDLISTSTRATEGYTASKSTORAGEDATABLOCK_H_

namespace pheet {

class LinkedListStrategyTaskStorageDataBlock {
public:
	LinkedListStrategyTaskStorageDataBlock(size_t id, View* first_view, Self* prev);
	~LinkedListStrategyTaskStorageDataBlock() {}

	T data[BlockSize];

	size_t id;
	View* first_view;
	Self* prev;
	Self* next;
	size_t filled;

	size_t active;
};

LinkedListStrategyTaskStorageDataBlock:LinkedListStrategyTaskStorageDataBlock(size_t id, View* first_view, Self* prev)
:id(id), first_view(first_view), prev(prev), next(nullptr), filled(0), active(BlockSize) {

}

}

#endif /* LINKEDLISTSTRATEGYTASKSTORAGEDATABLOCK_H_ */
