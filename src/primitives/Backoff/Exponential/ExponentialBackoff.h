/*
 * ExponentialBackoff.h
 *
 *  Created on: 18.04.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef EXPONENTIALBACKOFF_H_
#define EXPONENTIALBACKOFF_H_

#include <stdlib.h>
#include <time.h>
#include <algorithm>

/*
 *
 */
namespace pheet {

// TODO: switch to a thread-local random number generator. (otherwise backoff doesn't make much sense as random() contains a lock)
// Postponed until GCC has support for template aliases. Data-structures will probably be refactored then to always know the type of scheduler in use.

template <unsigned int MIN_BACKOFF = 100, unsigned int MAX_BACKOFF = 100000>
class ExponentialBackoff {
public:
	ExponentialBackoff();
	~ExponentialBackoff();

	void backoff();
private:
	unsigned int limit;

//	static thread_local boost::mt19937 rng;
};
/*
template <unsigned int MIN_BACKOFF, unsigned int MAX_BACKOFF>
thread_local boost::mt19937 ExponentialBackoff<MIN_BACKOFF, MAX_BACKOFF>::rng;
*/
template <unsigned int MIN_BACKOFF, unsigned int MAX_BACKOFF>
ExponentialBackoff<MIN_BACKOFF, MAX_BACKOFF>::ExponentialBackoff() {
	limit = MIN_BACKOFF;
}

template <unsigned int MIN_BACKOFF, unsigned int MAX_BACKOFF>
ExponentialBackoff<MIN_BACKOFF, MAX_BACKOFF>::~ExponentialBackoff() {
}

template <unsigned int MIN_BACKOFF, unsigned int MAX_BACKOFF>
void ExponentialBackoff<MIN_BACKOFF, MAX_BACKOFF>::backoff() {
//	boost::uniform_int<unsigned int> rnd_gen(0, limit);
	unsigned int sleep = random() % limit; //rnd_gen(rng);

	timespec delay;
	delay.tv_sec = (time_t)0;
	delay.tv_nsec = sleep;
	// let's sleep
	(void)nanosleep(&delay, (timespec *)NULL);

	limit = std::min(limit + sleep, MAX_BACKOFF);
}

typedef ExponentialBackoff<100, 100000> StandardExponentialBackoff;

}

#endif /* EXPONENTIALBACKOFF_H_ */
