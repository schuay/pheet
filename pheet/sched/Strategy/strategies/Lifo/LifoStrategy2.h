/*
 * LifoStrategy2.h
 *
 *  Created on: Mar 29, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LIFOSTRATEGY2_H_
#define LIFOSTRATEGY2_H_

#include <time.h>

namespace pheet {

template <class Pheet>
class LifoStrategy2 {
public:
	typedef LifoStrategy2<Pheet> Self;

	LifoStrategy2()
	:time(clock()) {}

	LifoStrategy2(Self& other)
	:time(other.time) {}

	LifoStrategy2(Self&& other)
	:time(other.time) {}

	~LifoStrategy2() {}

	bool prioritize(Self& other) {
		// Should work with wraparound (clock_t is unsigned)
		return (other.time - time) > 0;
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

#endif /* LIFOSTRATEGY2_H_ */
