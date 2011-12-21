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

template <class Scheduler, typename T, template <typename S> class Op = SumOperation>
class SumReducer {
public:
	SumReducer();
	SumReducer(SumReducer<Scheduler, T, Op>& other);
	~SumReducer();

	void incr();
	void decr();
	void add(T const& value);
	void sub(T const& value);

	T const& get_sum();
private:
	typedef OrderedReducer<Scheduler, ScalarMonoid<T, Op> > Reducer;
	Reducer reducer;
};

template <class Scheduler, typename T, template <typename S> class Op>
SumReducer<Scheduler, T, Op>::SumReducer() {

}

template <class Scheduler, typename T, template <typename S> class Op>
SumReducer<Scheduler, T, Op>::SumReducer(SumReducer<Scheduler, T, Op>& other)
: reducer(other.reducer) {

}

template <class Scheduler, typename T, template <typename S> class Op>
SumReducer<Scheduler, T, Op>::~SumReducer() {

}

template <class Scheduler, typename T, template <typename S> class Op>
void SumReducer<Scheduler, T, Op>::add(T const& value) {
	reducer.add_data(value);
}

template <class Scheduler, typename T, template <typename S> class Op>
void SumReducer<Scheduler, T, Op>::sub(T const& value) {
	reducer.add_data(-value);
}

template <class Scheduler, typename T, template <typename S> class Op>
void SumReducer<Scheduler, T, Op>::incr() {
	reducer.add_data(1);
}

template <class Scheduler, typename T, template <typename S> class Op>
void SumReducer<Scheduler, T, Op>::decr() {
	reducer.add_data(-1);
}

template <class Scheduler, typename T, template <typename S> class Op>
T const& SumReducer<Scheduler, T, Op>::get_sum() {
	return reducer.get_data();
}
}

#endif /* SUMREDUCER_H_ */
