/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "MspTests.h"

#include "Strategy/StrategyMspStrategy.h"

namespace pheet {

void MspTests::run_test() {
#ifdef MSP_TEST
	std::cout << "Hello World!" << std::endl;

	Pheet::Environment env;
	StrategyMspStrategy<Pheet, 3> strategy({1, 2, 3}); /* Just to catch syntax errors for now. */
#endif
}

} /* namespace pheet */
