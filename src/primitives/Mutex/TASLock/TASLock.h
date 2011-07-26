/*
 * TASLock.h
 *
 *  Created on: 22.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef TASLOCK_H_
#define TASLOCK_H_

namespace pheet {

class TASLock {
public:
	TASLock();
	~TASLock();

	void lock();
	bool try_lock(long int time_ms);

	void unlock();
private:
	int locked;
};

}

#endif /* TASLOCK_H_ */
