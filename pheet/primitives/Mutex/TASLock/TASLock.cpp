/*
 * TASLock.cpp
 *
 *  Created on: 22.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#include "TASLock.h"

namespace pheet {

TASLock::TASLock()
:locked(0) {

}

TASLock::~TASLock() {
}

void TASLock::lock() {
	while(!INT_CAS(&locked, 0, 1)) {};
}

bool TASLock::try_lock(long int time_ms) {
	struct timeval begin;
	gettimeofday(&begin, NULL);
	long int begin_ms = (begin.tv_usec / 1000) + (begin.tv_sec * 1000);
	while(!INT_CAS(&locked, 0, 1)) {
		struct timeval current;
		gettimeofday(&current, NULL);
		long int current_ms = (current.tv_usec / 1000) + (current.tv_sec * 1000);
		if(current_ms - begin_ms >= time_ms) {
			return false;
		}
	};
	return true;
}

void TASLock::unlock() {
	locked = 0;
}

}
