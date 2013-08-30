/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "ShortestPaths.h"

#include <stdio.h>

namespace sp
{

void
ShortestPaths::
print() const
{
	for (auto const & p : paths) {
		for (auto const & path : p.second) {
			path->print();
		}
		printf("\n");
	}
}

}
