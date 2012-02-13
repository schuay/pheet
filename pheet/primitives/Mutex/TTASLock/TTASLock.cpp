/*
 * TTASLock.cpp
 *
 *  Created on: 10.08.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#include "TTASLock.h"

namespace pheet {

TTASLock::TTASLock()
:locked(0) {

}

TTASLock::~TTASLock() {
}

void TTASLock::lock() {
	while(!INT_CAS(&locked, 0, 1)) {
		while(locked);
	};
}

bool TTASLock::try_lock(long int time_ms) {
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

void TTASLock::unlock() {
	locked = 0;
}

}
