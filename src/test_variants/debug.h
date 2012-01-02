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

const bool graph_bipartitioning_test = true;
const procs_t graph_bipartitioning_test_cpus[] = {1, 2};
const unsigned int graph_bipartitioning_test_seeds[] = {0, 1};
const GraphBipartitioningProblem graph_bipartitioning_test_problems[] = {
		// n, p, max_w
		{20, 0.5, 1000}
};
/*
const size_t graph_bipartitioning_test_n[] = {10};
const double graph_bipartitioning_test_p[] = {0.5};
const double graph_bipartitioning_test_max_w[] = {1, 1000};
*/
const int graph_bipartitioning_test_types[] = {0};

const bool inarow_test = true;
const procs_t inarow_test_cpus[] = {1, 2};
const unsigned int inarow_test_lookaheads[] = {3};

const bool    nqueens_test        = true;
const procs_t nqueens_test_cpus[] = {1, 2};
const size_t  nqueens_test_n[]    = {16};

}

#endif /* DEBUG_H_ */
