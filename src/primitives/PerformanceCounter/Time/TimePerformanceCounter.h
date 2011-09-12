/*
 * TimePerformanceCounter.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef TIMEPERFORMANCECOUNTER_H_
#define TIMEPERFORMANCECOUNTER_H_

#include <stdio.h>
#include <iostream>
#include <sys/time.h>

#include "../../../settings.h"
#include "../../Reducer/Sum/SumReducer.h"

/*
 *
 */
namespace pheet {

template <bool> class TimePerformanceCounter;

template <>
class TimePerformanceCounter<false> {
public:
	TimePerformanceCounter();
	TimePerformanceCounter(TimePerformanceCounter<false> const& other);
	~TimePerformanceCounter();

	void start_timer();
	void stop_timer();
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

inline
TimePerformanceCounter<false>::TimePerformanceCounter() {

}

inline
TimePerformanceCounter<false>::TimePerformanceCounter(TimePerformanceCounter<false> const& other) {

}

inline
TimePerformanceCounter<false>::~TimePerformanceCounter() {

}

inline
void TimePerformanceCounter<false>::start_timer() {

}

inline
void TimePerformanceCounter<false>::stop_timer() {

}

inline
void TimePerformanceCounter<false>::print(char const* const formatting_string) {

}

inline
void TimePerformanceCounter<false>::print_header(char const* const string) {

}

template <>
class TimePerformanceCounter<true> {
public:
	TimePerformanceCounter();
	TimePerformanceCounter(TimePerformanceCounter<true>& other);
	~TimePerformanceCounter();

	void start_timer();
	void stop_timer();
	void print(char const* formatting_string);
	static void print_header(char const* const string);
private:
	SumReducer<double> reducer;
	struct timeval start_time;
#ifndef NDEBUG
	bool is_active;
#endif
};

inline
TimePerformanceCounter<true>::TimePerformanceCounter()
#ifndef NDEBUG
: is_active(false)
#endif
{

}

inline
TimePerformanceCounter<true>::TimePerformanceCounter(TimePerformanceCounter<true>& other)
: reducer(other.reducer)
#ifndef NDEBUG
  , is_active(false)
#endif
{

}

inline
TimePerformanceCounter<true>::~TimePerformanceCounter() {

}

inline
void TimePerformanceCounter<true>::start_timer() {
#ifndef NDEBUG
	assert(!is_active);
	is_active = true;
#endif
	gettimeofday(&start_time, NULL);
}

inline
void TimePerformanceCounter<true>::stop_timer() {
	struct timeval stop_time;
	gettimeofday(&stop_time, NULL);
	double time = (stop_time.tv_sec - start_time.tv_sec) + 1.0e-6 * stop_time.tv_usec - 1.0e-6 * start_time.tv_usec;
	reducer.add(time);
#ifndef NDEBUG
	assert(is_active);
	is_active = false;
#endif
}

inline
void TimePerformanceCounter<true>::print(char const* const formatting_string) {
	printf(formatting_string, reducer.get_sum());
}

inline
void TimePerformanceCounter<true>::print_header(char const* const string) {
	std::cout << string;
}

}

#endif /* TIMEPERFORMANCECOUNTER_H_ */
