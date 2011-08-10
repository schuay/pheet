/*
 * BasicPerformanceCounter.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef BASICPERFORMANCECOUNTER_H_
#define BASICPERFORMANCECOUNTER_H_

#include "../../../../settings.h"
#include "../../Reducer/Sum/SumReducer.h"

/*
 *
 */
namespace pheet {

template <bool> class BasicPerformanceCounter;

template <>
class BasicPerformanceCounter<false> {
public:
	BasicPerformanceCounter();
	BasicPerformanceCounter(BasicPerformanceCounter const& other);
	~BasicPerformanceCounter();

	void incr();
};

template <>
BasicPerformanceCounter<false>::BasicPerformanceCounter() {

}

template <>
BasicPerformanceCounter<false>::BasicPerformanceCounter(BasicPerformanceCounter const& other) {

}

template <>
BasicPerformanceCounter<false>::~BasicPerformanceCounter() {

}

template <>
void BasicPerformanceCounter<false>::incr() {

}

template <>
class BasicPerformanceCounter<true> {
public:
	BasicPerformanceCounter();
	BasicPerformanceCounter(BasicPerformanceCounter<true> const& other);
	~BasicPerformanceCounter();

	void incr();
private:
	SumReducer<size_t> reducer;
};

template <>
BasicPerformanceCounter<true>::BasicPerformanceCounter() {

}

template <>
BasicPerformanceCounter<true>::BasicPerformanceCounter(BasicPerformanceCounter<true> const& other)
: reducer(other.reducer) {

}

template <>
BasicPerformanceCounter<true>::~BasicPerformanceCounter() {

}

template <>
void BasicPerformanceCounter<true>::incr() {
	reducer.incr();
}

}

#endif /* BASICPERFORMANCECOUNTER_H_ */
