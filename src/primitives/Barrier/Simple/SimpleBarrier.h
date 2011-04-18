/*
 * SimpleBarrier.h
 *
 *  Created on: 18.04.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef SIMPLEBARRIER_H_
#define SIMPLEBARRIER_H_

#include "../../../misc/types.h"

/*
 *
 */
namespace pheet {

template <class Backoff>
class SimpleBarrier {
public:
	SimpleBarrier();
	~SimpleBarrier();

	void wait(size_t i, procs_t p);
	void signal(size_t i);
	void barrier(size_t i, procs_t p);

private:
	procs_t barriers[4];
};

template <class Backoff>
SimpleBarrier<Backoff>::SimpleBarrier() {
	barriers[0] = 0;
	barriers[1] = 0;
	barriers[2] = 0;
	barriers[3] = 0;
}

template <class Backoff>
SimpleBarrier<Backoff>::~SimpleBarrier() {

}

template <class Backoff>
void SimpleBarrier<Backoff>::signal(size_t i) {
	INT_ATOMIC_ADD(&(barriers[i&3]), 1);
}

template <class Backoff>
void SimpleBarrier<Backoff>::wait(size_t i, procs_t p) {
	Backoff bo;
	while(barriers[i&3] != p) {
		bo.backoff();
	}
	barriers[(i+2) & 3] = 0;
}

template <class Backoff>
void SimpleBarrier<Backoff>::barrier(size_t i, procs_t p) {
	signal(i);
	wait(i, p);
}

}

#endif /* SIMPLEBARRIER_H_ */
