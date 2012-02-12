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

namespace pheet {

template <class Pheet, template <class P, typename T, typename Comp> class PriorityQueueT>
class DataStructuresEnv {
public:
	template<typename T, typename Comp/* = std::less<T>*/>
	using PriorityQueue = PriorityQueueT<Pheet, T, Comp>;
	template<typename T, typename Comp = std::less<T>>
	using PQ = PriorityQueueT<Pheet, T, Comp>;

	template<typename T, typename Comp = std::less<T>>
	using Heap = Heap<Pheet, T, Comp>;

};

template<class Pheet>
using DataStructures = DataStructuresEnv<Pheet, Heap>;

}

#endif /* DATASTRUCTURESENV_H_ */
