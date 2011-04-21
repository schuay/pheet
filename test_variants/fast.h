/*
 * fast.h
 *
 *  Created on: 11.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef FAST_H_
#define FAST_H_

#include "../src/misc/types.h"

namespace pheet {

const bool sorting_test = true;
const procs_t sorting_test_cpus[] = {1, 2, 4, 8, 16};
const unsigned int sorting_test_seeds[] = {0, 1, 2, 3, 4};
const size_t sorting_test_n[] = {10000000, 100000000, 8388607, 33554431};
const int sorting_test_types[] = {0, 1};

}

#endif /* FAST_H_ */
