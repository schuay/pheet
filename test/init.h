/*
 * init.h
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PHEET_TEST_INIT_H_
#define PHEET_TEST_INIT_H_

#include "settings.h"
#include <pheet/pheet.h>

namespace pheet {
struct GraphBipartitioningProblem {
	size_t n;
	double p;
	size_t max_w;
};

struct SetBenchProblem {
	size_t range;
	size_t blocks;
	double contains_p;
	double add_p;
};

}

#include ACTIVE_TEST

#endif /* PHEET_TEST_INIT_H_ */
