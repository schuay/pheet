/*
 * ConcurrentDataStructures.h
 *
 *  Created on: 12.02.2012
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef CONCURRENTDATASTRUCTURES_H_
#define CONCURRENTDATASTRUCTURES_H_

#include "../ds/CircularArray/FixedSize/FixedSizeCircularArray.h"
#include "../ds/CircularArray/TwoLevelGrowing/TwoLevelGrowingCircularArray.h"
#include "../ds/StealingDeque/CircularArray/CircularArrayStealingDeque.h"

namespace pheet {

template <class Pheet, template <class P, typename T> class CircularArrayT, template <class P, typename T> class FixedSizeCircularArrayT, template <class P, typename T> class StealingDequeT>
class ConcurrentDataStructuresEnv {
public:
	template<typename T>
		using CircularArray = CircularArrayT<Pheet, T>;
	template<typename T>
		using FixedSizeCircularArray = FixedSizeCircularArrayT<Pheet, T>;
	template<typename T>
		using StealingDeque = StealingDequeT<Pheet, T>;

	template<template <class P, typename T> class T>
		using WithStealingDeque = ConcurrentDataStructuresEnv<Pheet, CircularArrayT, FixedSizeCircularArrayT, T>;

	template <class P>
		using BT = ConcurrentDataStructuresEnv<P, CircularArrayT, FixedSizeCircularArrayT, StealingDequeT>;
};

template<class Pheet>
using ConcurrentDataStructures = ConcurrentDataStructuresEnv<Pheet, TwoLevelGrowingCircularArray, FixedSizeCircularArray, CircularArrayStealingDeque>;

}

#endif /* CONCURRENTDATASTRUCTURES_H_ */
