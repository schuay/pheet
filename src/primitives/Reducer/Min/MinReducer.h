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

template <typename T, template <typename S> class M = MinOperation >
class MinReducer : public MaxReducer<T, M> {
public:
	MinReducer();
	MinReducer(MinReducer<T, M>& other);
	~MinReducer();

	T const& get_min();
};

template <typename T, template <typename S> class M>
inline MinReducer<T, M>::MinReducer() {

}

template <typename T, template <typename S> class M>
inline MinReducer<T, M>::MinReducer(MinReducer<T, M>& other)
: MaxReducer<T, M>(other) {

}

template <typename T, template <typename S> class M>
inline MinReducer<T, M>::~MinReducer() {

}

template <typename T, template <typename S> class M>
inline T const& MinReducer<T, M>::get_min() {
	return MaxReducer<T, M>::get_max();
}

}

#endif /* MINREDUCER_H_ */
