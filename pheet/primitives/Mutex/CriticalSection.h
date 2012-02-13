/*
 * CriticalSection.h
 *
 *  Created on: 22.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

namespace pheet {

template <class Lock>
class CriticalSection {
public:
	CriticalSection(Lock *lock);
	~CriticalSection();

private:
	Lock* lock;
};

template <class Lock>
CriticalSection<Lock>::CriticalSection(Lock* lock) {
	lock->lock();
}

template <class Lock>
CriticalSection<Lock>::~CriticalSection() {
	lock->unlock();
}

}

#endif /* CRITICALSECTION_H_ */
