/*
 * FifoStrategy2.h
 *
 *  Created on: Mar 29, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef FIFOSTRATEGY2_H_
#define FIFOSTRATEGY2_H_

#include <time.h>

namespace pheet {

template <class Pheet>
class FifoStrategy2 {
public:
	typedef FifoStrategy2<Pheet> Self;

	FifoStrategy2()
	:time(clock()) {}

	FifoStrategy2(Self& other)
	:time(other.time) {}

	FifoStrategy2(Self&& other)
	:time(other.time) {}

	~FifoStrategy2() {}

	bool prioritize(Self& other) {
		// Should work with wraparound (clock_t is unsigned)
		return (time - other.time) > 0;
	}

	size_t get_transitive_weight() {
		return 32;
	}

	size_t get_memory_footprint() {
		return 32;
	}

private:
	clock_t time;
};

}

#endif /* FIFOSTRATEGY2_H_ */
