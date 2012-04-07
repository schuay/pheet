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

	size_t freed;
};

LinkedListStrategyTaskStorageDataBlock:LinkedListStrategyTaskStorageDataBlock(size_t id, View* first_view, Self* prev)
:id(id), first_view(first_view), prev(prev), next(nullptr), freed(0) {

}

}

#endif /* LINKEDLISTSTRATEGYTASKSTORAGEDATABLOCK_H_ */
