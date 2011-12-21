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

template <class Scheduler, bool> class TimePerformanceCounter;

template <class Scheduler>
class TimePerformanceCounter<Scheduler, false> {
public:
	TimePerformanceCounter();
	TimePerformanceCounter(TimePerformanceCounter<Scheduler, false> const& other);
	~TimePerformanceCounter();

	void start_timer();
	void stop_timer();
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

template <class Scheduler>
inline
TimePerformanceCounter<Scheduler, false>::TimePerformanceCounter() {

}

template <class Scheduler>
inline
TimePerformanceCounter<Scheduler, false>::TimePerformanceCounter(TimePerformanceCounter<Scheduler, false> const& other) {

}

template <class Scheduler>
inline
TimePerformanceCounter<Scheduler, false>::~TimePerformanceCounter() {

}

template <class Scheduler>
inline
void TimePerformanceCounter<Scheduler, false>::start_timer() {

}

template <class Scheduler>
inline
void TimePerformanceCounter<Scheduler, false>::stop_timer() {

}

template <class Scheduler>
inline
void TimePerformanceCounter<Scheduler, false>::print(char const* const formatting_string) {

}

template <class Scheduler>
inline
void TimePerformanceCounter<Scheduler, false>::print_header(char const* const string) {

}

template <class Scheduler>
class TimePerformanceCounter<Scheduler, true> {
public:
	TimePerformanceCounter();
	TimePerformanceCounter(TimePerformanceCounter<Scheduler, true>& other);
	~TimePerformanceCounter();

	void start_timer();
	void stop_timer();
	void print(char const* formatting_string);
	static void print_header(char const* const string);
private:
	SumReducer<Scheduler, double> reducer;
	struct timeval start_time;
#ifndef NDEBUG
	bool is_active;
#endif
};

template <class Scheduler>
inline
TimePerformanceCounter<Scheduler, true>::TimePerformanceCounter()
#ifndef NDEBUG
: is_active(false)
#endif
{

}

template <class Scheduler>
inline
TimePerformanceCounter<Scheduler, true>::TimePerformanceCounter(TimePerformanceCounter<Scheduler, true>& other)
: reducer(other.reducer)
#ifndef NDEBUG
  , is_active(false)
#endif
{

}

template <class Scheduler>
inline
TimePerformanceCounter<Scheduler, true>::~TimePerformanceCounter() {

}

template <class Scheduler>
inline
void TimePerformanceCounter<Scheduler, true>::start_timer() {
#ifndef NDEBUG
	assert(!is_active);
	is_active = true;
#endif
	gettimeofday(&start_time, NULL);
}

template <class Scheduler>
inline
void TimePerformanceCounter<Scheduler, true>::stop_timer() {
	struct timeval stop_time;
	gettimeofday(&stop_time, NULL);
	double time = (stop_time.tv_sec - start_time.tv_sec) + 1.0e-6 * stop_time.tv_usec - 1.0e-6 * start_time.tv_usec;
	reducer.add(time);
#ifndef NDEBUG
	assert(is_active);
	is_active = false;
#endif
}

template <class Scheduler>
inline
void TimePerformanceCounter<Scheduler, true>::print(char const* const formatting_string) {
#ifndef NDEBUG
	assert(!is_active);
#endif
	printf(formatting_string, reducer.get_sum());
}

template <class Scheduler>
inline
void TimePerformanceCounter<Scheduler, true>::print_header(char const* const string) {
	std::cout << string;
}

}

#endif /* TIMEPERFORMANCECOUNTER_H_ */
