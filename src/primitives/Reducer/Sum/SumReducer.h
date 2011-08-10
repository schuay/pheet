/*
 * SumReducer.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef SUMREDUCER_H_
#define SUMREDUCER_H_

#include "../Ordered/OrderedReducer.h"

/*
 *
 */
namespace pheet {

template <typename T>
struct SumOperation {
	T operator()(T x, T y);
};

template <typename T>
T SumOperation::operator ()(T x, T y) {
	return x + y;
}

template <typename T>
class SumReducer {
public:
	SumReducer();
	~SumReducer();

private:
	typedef OrderedReducer<T, SumOperation> Reducer;
	Reducer reducer;
};

template <typename T>
SumReducer<T>::SumReducer() {

}

template <typename T>
SumReducer<T>::SumReducer(SumReducer<T> const& other)
: reducer(other.reducer) {

}

template <typename T>
SumReducer<T>::~SumReducer() {

}

}

#endif /* SUMREDUCER_H_ */
