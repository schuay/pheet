/*
 * MinReducer.h
 *
 *  Created on: Nov 7, 2011
 *      Author: mwimmer
 */

#ifndef MINREDUCER_H_
#define MINREDUCER_H_

#include "../Max/MaxReducer.h"

namespace pheet {

template <typename T>
struct MinOperation {
	T operator()(T x, T y);
	T get_identity();
};

template <typename T>
T MinOperation<T>::operator()(T x, T y) {
	return std::min(x, y);
}

template <typename T>
T MinOperation<T>::get_identity() {
	return std::numeric_limits<T>::max();
}

template <class Scheduler, typename T, template <typename S> class M = MinOperation >
class MinReducer : public MaxReducer<Scheduler, T, M> {
public:
	MinReducer();
	MinReducer(MinReducer<Scheduler, T, M>& other);
	~MinReducer();

	T const& get_min();
};

template <class Scheduler, typename T, template <typename S> class M>
inline MinReducer<Scheduler, T, M>::MinReducer() {

}

template <class Scheduler, typename T, template <typename S> class M>
inline MinReducer<Scheduler, T, M>::MinReducer(MinReducer<Scheduler, T, M>& other)
: MaxReducer<Scheduler, T, M>(other) {

}

template <class Scheduler, typename T, template <typename S> class M>
inline MinReducer<Scheduler, T, M>::~MinReducer() {

}

template <class Scheduler, typename T, template <typename S> class M>
inline T const& MinReducer<Scheduler, T, M>::get_min() {
	return MaxReducer<Scheduler, T, M>::get_max();
}

}

#endif /* MINREDUCER_H_ */
