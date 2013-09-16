/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <list>

#include "Path.h"

namespace sp
{

#define PATHMM_INITIAL_SIZE (128)

template <class Pheet>
class PathMM
{
public:
	PathMM()
		: size(PATHMM_INITIAL_SIZE), current(0) {
		areas.push_back(new Path[size]);
	}

	~PathMM() {
		for (PathPtr p : areas) {
			delete[] p;
		}
	}

	PathPtr
	acquire() {
		if (current == size) {
			size <<= 1;
			current = 0;
			areas.push_back(new Path[size]);
		}

		return areas.back() + current;
	}

private:
	std::list<PathPtr> areas;
	size_t size;
	size_t current;
};

}
