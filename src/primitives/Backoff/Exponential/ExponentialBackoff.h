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

template <unsigned int MIN_BACKOFF = 100, unsigned int MAX_BACKOFF = 100000>
class ExponentialBackoff {
public:
	ExponentialBackoff();
	~ExponentialBackoff();

	void backoff();
private:
	unsigned int limit;
};

template <unsigned int MIN_BACKOFF, unsigned int MAX_BACKOFF>
ExponentialBackoff<MIN_BACKOFF, MAX_BACKOFF>::ExponentialBackoff() {
	limit = MIN_BACKOFF;
}

template <unsigned int MIN_BACKOFF, unsigned int MAX_BACKOFF>
ExponentialBackoff<MIN_BACKOFF, MAX_BACKOFF>::~ExponentialBackoff() {
}

template <unsigned int MIN_BACKOFF, unsigned int MAX_BACKOFF>
void ExponentialBackoff<MIN_BACKOFF, MAX_BACKOFF>::backoff() {
	unsigned int sleep = random() % limit;

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
