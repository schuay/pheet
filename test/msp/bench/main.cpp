/*
 * Copyright Martin Wimmer 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define NDEBUG 1

#include "MspTests.h"

using namespace pheet;

int main(int, char**)
{
	MspTests t;
	t.run_test();

	return 0;
}