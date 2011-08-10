/*
 * MaxReducer.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef MAXREDUCER_H_
#define MAXREDUCER_H_

/*
 *
 */
namespace pheet {

template <typename T>
struct MaxOperation {
	T operator()(T x, T y);
};

template <typename T>
T MaxOperation::operator ()(T x, T y) {
	return max(x, y);
}

template <typename T>
class MaxReducer {
public:
	MaxReducer();
	MaxReducer(MaxReducer<T> const& other);
	~MaxReducer();

	void add_value(T value);
	T get_max();
private:
	typedef OrderedReducer<T, MaxOperation> Reducer;
	Reducer reducer;
};

template <typename T>
MaxReducer<T>::MaxReducer() {

}

template <typename T>
MaxReducer<T>::MaxReducer(MaxReducer<T> const& other)
: reducer(other.reducer) {

}

template <typename T>
MaxReducer<T>::~MaxReducer() {

}

template <typename T>
void MaxReducer<T>::add_value(T value) {
	reducer.add_data(value);
}

template <typename T>
T MaxReducer<T>::get_max() {
	return reducer.get_data();
}

}

#endif /* MAXREDUCER_H_ */
