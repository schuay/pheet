/*
 * BasicPerformanceCounter.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef BASICPERFORMANCECOUNTER_H_
#define BASICPERFORMANCECOUNTER_H_

#include <stdio.h>

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
	BasicPerformanceCounter(BasicPerformanceCounter<false> const& other);
	~BasicPerformanceCounter();

	void incr();
	void output(char const* const formatting_string);
};

BasicPerformanceCounter<false>::BasicPerformanceCounter() {

}

BasicPerformanceCounter<false>::BasicPerformanceCounter(BasicPerformanceCounter<false> const& other) {

}

BasicPerformanceCounter<false>::~BasicPerformanceCounter() {

}

void BasicPerformanceCounter<false>::incr() {

}

void BasicPerformanceCounter<false>::output(char const* const formatting_string) {

}

template <>
class BasicPerformanceCounter<true> {
public:
	BasicPerformanceCounter();
	BasicPerformanceCounter(BasicPerformanceCounter<true>& other);
	~BasicPerformanceCounter();

	void incr();
	void output(char const* formatting_string);
private:
	SumReducer<size_t> reducer;
};

BasicPerformanceCounter<true>::BasicPerformanceCounter() {

}

BasicPerformanceCounter<true>::BasicPerformanceCounter(BasicPerformanceCounter<true>& other)
: reducer(other.reducer) {

}

BasicPerformanceCounter<true>::~BasicPerformanceCounter() {

}

void BasicPerformanceCounter<true>::incr() {
	reducer.incr();
}

void BasicPerformanceCounter<true>::output(char const* const formatting_string) {
	printf(formatting_string, reducer.get_sum());
}


}

#endif /* BASICPERFORMANCECOUNTER_H_ */
