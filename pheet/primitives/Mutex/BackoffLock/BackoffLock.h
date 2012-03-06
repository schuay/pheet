/*
 * TASLock.h
 *
 *  Created on: 10.08.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef BACKOFFLOCK_H_
#define BACKOFFLOCK_H_

namespace pheet {

template <class Pheet, class Backoff>
class BackoffLockImpl {
public:
	template <class NewBO>
	using WithBackoff = BackoffLockImpl<Pheet, NewBO>;

	template <class P>
	using T = BackoffLockImpl<P, Backoff>;

	BackoffLockImpl();
	~BackoffLockImpl();

	void lock();
	bool try_lock();
	bool try_lock(long int time_ms);

	void unlock();
private:
	int volatile locked;
};

template <class Pheet, class Backoff>
BackoffLockImpl<Pheet, Backoff>::BackoffLock()
:locked(0) {

}

template <class Pheet, class Backoff>
BackoffLockImpl<Pheet, Backoff>::~BackoffLock() {
}

template <class Pheet, class Backoff>
void BackoffLockImpl<Pheet, Backoff>::lock() {
	Backoff bo;
	while(!INT_CAS(&locked, 0, 1)) {
		while(locked) {
			bo.backoff();
		}
	};
}

template <class Pheet, class Backoff>
bool BackoffLockImpl<Pheet, Backoff>::try_lock() {
	return INT_CAS(&locked, 0, 1);
}

template <class Pheet, class Backoff>
bool BackoffLockImpl<Pheet, Backoff>::try_lock(long int time_ms) {
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

template <class Pheet, class Backoff>
void BackoffLockImpl<Pheet, Backoff>::unlock() {
	locked = 0;
}

template <class Pheet>
using BackoffLock = BackoffLockImpl<Pheet, Pheet::Backoff>;

}

#endif /* TASLOCK_H_ */
