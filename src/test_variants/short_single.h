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

#define SORTING_TEST true
const procs_t sorting_test_cpus[] = {1, 2, 4, 8};
const unsigned int sorting_test_seeds[] = {0};
const size_t sorting_test_n[] = {10000000};
const int sorting_test_types[] = {0};

const bool graph_bipartitioning_test = true;
const procs_t graph_bipartitioning_test_cpus[] = {1, 2, 4, 8};
const unsigned int graph_bipartitioning_test_seeds[] = {0};
const GraphBipartitioningProblem graph_bipartitioning_test_problems[] = {
		// n, p, max_w
		{35, 0.5, 1000}
};
/*
const size_t graph_bipartitioning_test_n[] = {35};
const double graph_bipartitioning_test_p[] = {0.5};
const double graph_bipartitioning_test_max_w[] = {1000};
*/
const int graph_bipartitioning_test_types[] = {0};

#define INAROW_TEST true
const procs_t inarow_test_cpus[] = {1, 2, 4, 8};
const unsigned int inarow_test_lookaheads[] = {5};

const bool    nqueens_test        = true;
const procs_t nqueens_test_cpus[] = {1, 2, 4, 8};
const size_t  nqueens_test_n[]    = {24};

#define LUPIV_TEST	true
const procs_t lupiv_test_cpus[] = {1, 2, 4, 8};
const unsigned int lupiv_test_seeds[] = {0};
const size_t lupiv_test_n[] = {1024};
const int lupiv_test_types[] = {0};

}

#endif /* SHORT_SINGLE_H_ */
