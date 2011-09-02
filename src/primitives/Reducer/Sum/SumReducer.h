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
#include "../Ordered/ScalarMonoid.h"
#include "SumOperation.h"

/*
 *
 */
namespace pheet {

template <typename T, template <typename S> class Op = SumOperation>
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
	typedef OrderedReducer<ScalarMonoid<T, SumOperation> > Reducer;
	Reducer reducer;
};

template <typename T, template <typename S> class Op>
SumReducer<T, Op>::SumReducer() {

}

template <typename T, template <typename S> class Op>
SumReducer<T, Op>::SumReducer(SumReducer<T>& other)
: reducer(other.reducer) {

}

template <typename T, template <typename S> class Op>
SumReducer<T, Op>::~SumReducer() {

}

template <typename T, template <typename S> class Op>
void SumReducer<T, Op>::add(T value) {
	reducer.add_data(value);
}

template <typename T, template <typename S> class Op>
void SumReducer<T, Op>::sub(T value) {
	reducer.add_data(-value);
}

template <typename T, template <typename S> class Op>
void SumReducer<T, Op>::incr() {
	reducer.add_data(1);
}

template <typename T, template <typename S> class Op>
void SumReducer<T, Op>::decr() {
	reducer.add_data(-1);
}

template <typename T, template <typename S> class Op>
T SumReducer<T, Op>::get_sum() {
	return reducer.get_data();
}
}

#endif /* SUMREDUCER_H_ */
