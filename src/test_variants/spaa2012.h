/*
 * spaa2012.h
 *
 *  Created on: 11.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SPAA2012_H_
#define SPAA2012_H_

#include "../misc/types.h"

namespace pheet {

const bool sorting_test = false;
const procs_t sorting_test_cpus[] = {1, 2, 4, 8, 16, 32, 64, 128};
const unsigned int sorting_test_seeds[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const size_t sorting_test_n[] = {10000000, 100000000, 1000000000, 8388607, 33554431, 134217727};
const int sorting_test_types[] = {0, 1, 3, 4};

const bool graph_bipartitioning_test = true;
const procs_t graph_bipartitioning_test_cpus[] = {1, 2, 4, 8, 16, 32, 64, 128};
const unsigned int graph_bipartitioning_test_seeds[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const GraphBipartitioningProblem graph_bipartitioning_test_problems[] = {
		// n, p, max_w
		{42, 0.5, 1},
		{42, 0.5, 1000},
};
const int graph_bipartitioning_test_types[] = {0};

const bool    nqueens_test        = false;
const procs_t nqueens_test_cpus[] = {1, 2, 4, 8, 16, 32, 64, 128};
const size_t  nqueens_test_n[]    = {24, 32, 48};

#define LUPIV_TEST	true
const procs_t lupiv_test_cpus[] = {1, 2, 4, 8, 16, 32, 64, 128};
const unsigned int lupiv_test_seeds[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const size_t lupiv_test_n[] = {1024, 2048};
const int lupiv_test_types[] = {0};

}

#endif /* SPAA2012_H_ */
