/*
 * full.h
 *
 *  Created on: 11.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef FULL_H_
#define FULL_H_

#include "../misc/types.h"

namespace pheet {

const bool sorting_test = true;
const procs_t sorting_test_cpus[] = {1, 2, 4, 8, 16, 32, 64, 128};
const unsigned int sorting_test_seeds[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const size_t sorting_test_n[] = {10000000, 100000000, 1000000000, 8388607, 33554431, 134217727};
const int sorting_test_types[] = {0, 1, 3, 4};

const bool graph_bipartitioning_test = false;
const procs_t graph_bipartitioning_test_cpus[] = {1, 2, 4, 8, 16};
const unsigned int graph_bipartitioning_test_seeds[] = {0, 1, 2, 3, 4};
const size_t graph_bipartitioning_test_n[] = {10, 12, 14, 16};
const double graph_bipartitioning_test_p[] = {0.1, 0.5, 0.9};
const int graph_bipartitioning_test_types[] = {0};

}

#endif /* FULL_H_ */
