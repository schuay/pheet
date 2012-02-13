/*
 * TASLock.h
 *
 *  Created on: 10.08.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef BACKOFFLOCK_H_
#define BACKOFFLOCK_H_

#include "../../Backoff/Exponential/ExponentialBackoff.h"

namespace pheet {

template <class Backoff = ExponentialBackoff>
class BackoffLock {
public:
	BackoffLock();
	~BackoffLock();

	void lock();
	bool try_lock(long int time_ms);

	void unlock();
private:
	int locked;
};

template <class Backoff>
BackoffLock<Backoff>::BackoffLock()
:locked(0) {

}

template <class Backoff>
BackoffLock<Backoff>::~BackoffLock() {
}

template <class Backoff>
void BackoffLock<Backoff>::lock() {
	Backoff bo;
	while(!INT_CAS(&locked, 0, 1)) {
		while(locked) {
			bo.backoff();
		}
	};
}

template <class Backoff>
bool BackoffLock<Backoff>::try_lock(long int time_ms) {
	Backoff bo;
	struct timeval begin;
	gettimeofday(&begin, NULL);
	long int begin_ms = (begin.tv_usec / 1000) + (begin.tv_sec * 1000);
	while(!INT_CAS(&locked, 0, 1)) {
		do {
			struct timeval current;
			gettimeofday(&current, NULL);
			long int current_ms = (current.tv_usec / 1000) + (current.tv_sec * 1000);
			if(current_ms - begin_ms >= time_ms) {
				return false;
			}
			bo.backoff();
		}while(locked);
	};
	return true;
}

template <class Backoff>
void BackoffLock<Backoff>::unlock() {
	locked = 0;
}

}

#endif /* TASLOCK_H_ */
