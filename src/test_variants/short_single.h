/*
 * full.h
 *
 *  Created on: 11.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SHORT_SINGLE_H_
#define SHORT_SINGLE_H_

#include "../misc/types.h"

namespace pheet {

const bool sorting_test = true;
const procs_t sorting_test_cpus[] = {1, 2, 4, 8, 16, 32};
const unsigned int sorting_test_seeds[] = {0};
const size_t sorting_test_n[] = {10000000};
const int sorting_test_types[] = {0};

const bool graph_bipartitioning_test = true;
const procs_t graph_bipartitioning_test_cpus[] = {1, 2, 4, 8, 16, 32};
const unsigned int graph_bipartitioning_test_seeds[] = {0};
const size_t graph_bipartitioning_test_n[] = {24};
const double graph_bipartitioning_test_p[] = {0.5};
const int graph_bipartitioning_test_types[] = {0};

const bool inarow_test = false;
}

#endif /* SHORT_SINGLE_H_ */
