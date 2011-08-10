/*
 * MaxPerformanceCounter.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef MAXPERFORMANCECOUNTER_H_
#define MAXPERFORMANCECOUNTER_H_

#include "../../../../settings.h"
#include "../../Reducer/Max/MaxReducer.h"

/*
 *
 */
namespace pheet {

template <typename T, bool> class MaxPerformanceCounter;

template <typename T>
class MaxPerformanceCounter<T, false> {
public:
	MaxPerformanceCounter();
	MaxPerformanceCounter(MaxPerformanceCounter const& other);
	~MaxPerformanceCounter();

	void add_value();
};

template <typename T>
MaxPerformanceCounter<T, false>::MaxPerformanceCounter() {

}

template <typename T>
MaxPerformanceCounter<T, false>::MaxPerformanceCounter(MaxPerformanceCounter const& other) {

}

template <typename T>
MaxPerformanceCounter<T, false>::~MaxPerformanceCounter() {

}

template <typename T>
void MaxPerformanceCounter<T, false>::incr() {

}

template <typename T>
class MaxPerformanceCounter<T, true> {
public:
	MaxPerformanceCounter();
	MaxPerformanceCounter(MaxPerformanceCounter<true> const& other);
	~MaxPerformanceCounter();

	void incr();
private:
	MaxReducer<size_t> reducer;
};

template <typename T>
MaxPerformanceCounter<T, true>::MaxPerformanceCounter() {

}

template <typename T>
MaxPerformanceCounter<T, true>::MaxPerformanceCounter(MaxPerformanceCounter<true> const& other)
: reducer(other.reducer) {

}

template <typename T>
MaxPerformanceCounter<T, true>::~MaxPerformanceCounter() {

}

template <typename T>
void MaxPerformanceCounter<T, true>::incr() {
	reducer.incr();
}

}

#endif /* MAXPERFORMANCECOUNTER_H_ */
