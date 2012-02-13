/*
 * fast.h
 *
 *  Created on: 11.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef FAST_H_
#define FAST_H_

#include "pheet/misc/types.h"

namespace pheet {

#define SORTING_TEST true
const procs_t sorting_test_cpus[] = {1, 2, 4, 8, 16};
const unsigned int sorting_test_seeds[] = {0, 1, 2, 3, 4};
const size_t sorting_test_n[] = {10000000, 100000000, 8388607, 33554431};
const int sorting_test_types[] = {0, 1};

const bool graph_bipartitioning_test = true;
const procs_t graph_bipartitioning_test_cpus[] = {1, 2, 4, 8, 16};
const unsigned int graph_bipartitioning_test_seeds[] = {0, 1, 2, 3, 4};
const GraphBipartitioningProblem graph_bipartitioning_test_problems[] = {
		// n, p, max_w
		{35, 0.5, 1000}
};
/*
const size_t graph_bipartitioning_test_n[] = {20, 22, 24};
const double graph_bipartitioning_test_p[] = {0.1, 0.5, 0.9};
const double graph_bipartitioning_test_max_w[] = {1, 1000};
*/
const int graph_bipartitioning_test_types[] = {0};

#define INAROW_TEST true
const procs_t inarow_test_cpus[] = {1, 2, 4, 8, 16};
const unsigned int inarow_test_lookaheads[] = {5};

const bool    nqueens_test        = true;
const procs_t nqueens_test_cpus[] = {1, 2, 4, 8, 16};
const size_t  nqueens_test_n[]    = {24, 32};

}

#endif /* FAST_H_ */
