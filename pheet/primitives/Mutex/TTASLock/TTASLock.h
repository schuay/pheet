/*
 * TASLock.h
 *
 *  Created on: 10.08.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef TTASLOCK_H_
#define TTASLOCK_H_

namespace pheet {

template <class Pheet>
class TTASLock {
public:
	TTASLock();
	~TTASLock();

	void lock();
	bool try_lock();
	bool try_lock(long int time_ms);

	void unlock();
private:
	// Volatile needed to ensure the compiler doesn't optimize the while(locked) loop
	int volatile locked;
};

template <class Pheet>
TTASLock<Pheet>::TTASLock()
:locked(0) {

}

template <class Pheet>
TTASLock<Pheet>::~TTASLock() {
}

template <class Pheet>
void TTASLock<Pheet>::lock() {
	do {
		while(locked);
	}
	while(!INT_CAS(&locked, 0, 1));
}

template <class Pheet>
bool TTASLock<Pheet>::try_lock() {
	return (!locked) && INT_CAS(&locked, 0, 1);
}

template <class Pheet>
bool TTASLock<Pheet>::try_lock(long int time_ms) {
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
		}while(locked);
	};
	return true;
}

template <class Pheet>
void TTASLock<Pheet>::unlock() {
	locked = 0;
}

}

#endif /* TASLOCK_H_ */
