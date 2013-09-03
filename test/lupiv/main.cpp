/*
 * Copyright Martin Wimmer 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define NDEBUG 1

#include "../init.h"
#include "LUPivTests.h"

#define USES_STRATEGY (true)

using namespace pheet;

int main(int, char**)
{
	LUPivTests t;
	t.run_test(USES_STRATEGY);

	return 0;
}