/*
 * DataStructuresEnv.h
 *
 *  Created on: Jan 31, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef DATASTRUCTURESENV_H_
#define DATASTRUCTURESENV_H_

#include "../ds/PriorityQueue/Heap/Heap.h"
#include "../ds/PriorityDeque/DeHeap/DeHeap.h"

namespace pheet {

template <class Pheet, template <class P, typename T, typename Comp> class PriorityQueueT, template <class P, typename T, typename Comp> class PriorityDequeT>
class DataStructuresEnv {
public:
	template<typename T, typename Comp = std::less<T>>
		using PriorityQueue = PriorityQueueT<Pheet, T, Comp>;
	template<typename T, typename Comp = std::less<T>>
		using PQ = PriorityQueueT<Pheet, T, Comp>;

	template<typename T, typename Comp = std::less<T>>
		using PriorityDeque = PriorityDequeT<Pheet, T, Comp>;
	template<typename T, typename Comp = std::less<T>>
		using PDQ = PriorityDequeT<Pheet, T, Comp>;

	template<typename T, typename Comp = std::less<T>>
		using Heap = Heap<Pheet, T, Comp>;

};

template<class Pheet>
using DataStructures = DataStructuresEnv<Pheet, Heap, DeHeap>;

}

#endif /* DATASTRUCTURESENV_H_ */
