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
	T get_identity();
};

template <typename T>
T SumOperation<T>::operator()(T x, T y) {
	return x + y;
}

template <typename T>
T SumOperation<T>::get_identity() {
	return 0;
}

template <typename T>
class SumReducer {
public:
	SumReducer();
	SumReducer(SumReducer<T>& other);
	~SumReducer();

	void incr();
	void decr();
	void add(T value);
	void sub(T value);

	T get_sum();
private:
	typedef OrderedReducer<T, SumOperation> Reducer;
	Reducer reducer;
};

template <typename T>
SumReducer<T>::SumReducer() {

}

template <typename T>
SumReducer<T>::SumReducer(SumReducer<T>& other)
: reducer(other.reducer) {

}

template <typename T>
SumReducer<T>::~SumReducer() {

}

template <typename T>
void SumReducer<T>::add(T value) {
	reducer.add_data(value);
}

template <typename T>
void SumReducer<T>::sub(T value) {
	reducer.add_data(-value);
}

template <typename T>
void SumReducer<T>::incr() {
	reducer.add_data(1);
}

template <typename T>
void SumReducer<T>::decr() {
	reducer.add_data(-1);
}

template <typename T>
T SumReducer<T>::get_sum() {
	return reducer.get_data();
}
}

#endif /* SUMREDUCER_H_ */
