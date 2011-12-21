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

const bool sorting_test = false;
const procs_t sorting_test_cpus[] = {1, 2, 4, 8};
const unsigned int sorting_test_seeds[] = {0};
const size_t sorting_test_n[] = {10000000};
const int sorting_test_types[] = {0};

const bool graph_bipartitioning_test = false;
const procs_t graph_bipartitioning_test_cpus[] = {1, 2, 4, 8};
const unsigned int graph_bipartitioning_test_seeds[] = {0};
const size_t graph_bipartitioning_test_n[] = {24};
const double graph_bipartitioning_test_p[] = {0.5};
const int graph_bipartitioning_test_types[] = {0};

const bool inarow_test = false;
const procs_t inarow_test_cpus[] = {4};
const unsigned int inarow_test_lookaheads[] = {7};

const bool    nqueens_test        = false;
const procs_t nqueens_test_cpus[] = {1, 2, 4, 8};
const size_t  nqueens_test_n[]    = {24};


const bool uts_test = false;
const procs_t uts_test_cpus[] = {4};

// 0 # (T1) Geometric [fixed] ------- Tree size = 4130071, tree depth = 10, num leaves = 3305118 (80.03%)
// 1 # (T5) Geometric [linear dec.] - Tree size = 4147582, tree depth = 20, num leaves = 2181318 (52.59%)
// 2 # (T2) Geometric [cyclic] ------ Tree size = 4117769, tree depth = 81, num leaves = 2342762 (56.89%)
// 3 # (T3) Binomial ---------------- Tree size = 4112897, tree depth = 1572, num leaves = 3599034 (87.51%)
// 4 # (T4) Hybrid ------------------ Tree size = 4132453, tree depth = 134, num leaves = 3108986 (75.23%)
// 5 # (T1L) Geometric [fixed] ------ Tree size = 102181082, tree depth = 13, num leaves = 81746377 (80.00%)
// 6 # (T2L) Geometric [cyclic] ----- Tree size = 96793510, tree depth = 67, num leaves = 53791152 (55.57%)
// 7 # (T3L) Binomial --------------- Tree size = 111345631, tree depth = 17844, num leaves = 89076904 (80.00%)
// 8 # (T1XL) Geometric [fixed] ----- Tree size = 1635119272, tree depth = 15, num leaves = 1308100063 (80.00%)
const unsigned int uts_test_standardworkloads[] = {0,1,2,3,4,5,6,7,8};

const bool sor_test = true;
const procs_t sor_test_cpus[] = {4};
const int sor_test_maxtrissize[] = {1000,1500,2000,16000};
const int sor_test_iterations[] =  {100};
const int sor_test_slices[] = {32};
const double sor_test_omega[] = {1.25};
}

#endif /* SHORT_SINGLE_H_ */
