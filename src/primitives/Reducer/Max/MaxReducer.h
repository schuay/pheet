/*
 * MaxReducer.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef MAXREDUCER_H_
#define MAXREDUCER_H_

#include "../Ordered/OrderedReducer.h"
#include "../Ordered/ScalarMonoid.h"

#include <limits>

/*
 *
 */
namespace pheet {

template <typename T>
struct MaxOperation {
	T operator()(T x, T y);
	T get_identity();
};

template <typename T>
T MaxOperation<T>::operator()(T x, T y) {
	return max(x, y);
}

template <typename T>
T MaxOperation<T>::get_identity() {
	return std::numeric_limits<T>::max();
}

template <typename T, template <typename S> class M = MaxOperation >
class MaxReducer {
public:
	MaxReducer();
	MaxReducer(MaxReducer<T> const& other);
	~MaxReducer();

	void add_value(T const& value);
	T const& get_max();
private:
	typedef OrderedReducer<ScalarMonoid<T, M> > Reducer;
	Reducer reducer;
};

template <typename T, template <typename S> class M>
MaxReducer<T, M>::MaxReducer() {

}

template <typename T, template <typename S> class M>
MaxReducer<T, M>::MaxReducer(MaxReducer<T> const& other)
: reducer(other.reducer) {

}

template <typename T, template <typename S> class M>
MaxReducer<T, M>::~MaxReducer() {

}

template <typename T, template <typename S> class M>
void MaxReducer<T, M>::add_value(T const& value) {
	reducer.add_data(value);
}

template <typename T, template <typename S> class M>
T const& MaxReducer<T, M>::get_max() {
	return reducer.get_data();
}

}

#endif /* MAXREDUCER_H_ */
