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

class TTASLock {
public:
	TTASLock();
	~TTASLock();

	void lock();
	bool try_lock(long int time_ms);

	void unlock();
private:
	// Volatile needed to ensure the compiler doesn't optimize the while(locked) loop
	int volatile locked;
};

}

#endif /* TASLOCK_H_ */
