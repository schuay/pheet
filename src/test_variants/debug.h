/*
 * debug.h
 *
 *  Created on: 25.07.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "../misc/types.h"

namespace pheet {

const bool sorting_test = true;
const procs_t sorting_test_cpus[] = {1, 2};
const unsigned int sorting_test_seeds[] = {0, 1};
const size_t sorting_test_n[] = {100000};
const int sorting_test_types[] = {0, 1};

}

#endif /* DEBUG_H_ */
