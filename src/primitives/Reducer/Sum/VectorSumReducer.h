/*
 * VectorSumReducer.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef VECTORSUMREDUCER_H_
#define VECTORSUMREDUCER_H_

#include "../Ordered/OrderedReducer.h"
#include "../Ordered/VectorMonoid.h"
#include "SumOperation.h"

/*
 *
 */
namespace pheet {

template <typename T, template <typename S> class Op = SumOperation>
class VectorSumReducer {
public:
	VectorSumReducer(size_t length);
	VectorSumReducer(VectorSumReducer<T, Op>& other);
	~VectorSumReducer();

	void incr(size_t i);
	void decr(size_t i);
	void add(size_t i, T value);
	void sub(size_t i, T value);

	T const* get_sum();
private:
	typedef OrderedReducer<VectorMonoid<T, Op> > Reducer;
	Reducer reducer;
};

template <typename T, template <typename S> class Op>
VectorSumReducer<T, Op>::VectorSumReducer(size_t length)
: reducer(length) {

}

template <typename T, template <typename S> class Op>
VectorSumReducer<T, Op>::VectorSumReducer(VectorSumReducer<T, Op>& other)
: reducer(other.reducer) {

}

template <typename T, template <typename S> class Op>
VectorSumReducer<T, Op>::~VectorSumReducer() {

}

template <typename T, template <typename S> class Op>
void VectorSumReducer<T, Op>::add(size_t i, T value) {
	reducer.add_data(i, value);
}

template <typename T, template <typename S> class Op>
void VectorSumReducer<T, Op>::sub(size_t i, T value) {
	reducer.add_data(i, -value);
}

template <typename T, template <typename S> class Op>
void VectorSumReducer<T, Op>::incr(size_t i) {
	reducer.add_data(i, 1);
}

template <typename T, template <typename S> class Op>
void VectorSumReducer<T, Op>::decr(size_t i) {
	reducer.add_data(i, -1);
}

template <typename T, template <typename S> class Op>
T const* VectorSumReducer<T, Op>::get_sum() {
	return reducer.get_data();
}

}

#endif /* VECTORSUMREDUCER_H_ */
