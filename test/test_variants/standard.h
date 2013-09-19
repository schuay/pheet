/*
 * full.h
 *
 *  Created on: 11.04.2011
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef SHORT_SINGLE_H_
#define SHORT_SINGLE_H_

#include "pheet/misc/types.h"

namespace pheet {

#define SORTING_TEST true
const procs_t sorting_test_cpus[] = {1, 2, 4, 8};
const unsigned int sorting_test_seeds[] = {0};
const size_t sorting_test_n[] = {10000000};
const int sorting_test_types[] = {0};

#define GRAPH_BIPARTITIONING_TEST true
const procs_t graph_bipartitioning_test_cpus[] = {1, 2, 4, 8};
const unsigned int graph_bipartitioning_test_seeds[] = {0};
const GraphBipartitioningProblem graph_bipartitioning_test_problems[] = {
		// n, p, max_w
		{35, 0.5, 1000}
};
const int graph_bipartitioning_test_types[] = {0};

//#define INAROW_TEST true
const procs_t inarow_test_cpus[] = {1, 2, 4, 8};
const unsigned int inarow_test_lookaheads[] = {5};

//#define NQUEENS_TEST true
const procs_t nqueens_test_cpus[] = {1, 2, 4, 8};
const size_t  nqueens_test_n[]    = {24};

//#define LUPIV_TEST	true
const procs_t lupiv_test_cpus[] = {1, 2, 4, 8};
const unsigned int lupiv_test_seeds[] = {0};
const size_t lupiv_test_n[] = {1024};
const int lupiv_test_types[] = {0};

//#define PREFIX_SUM_TEST true
const procs_t prefix_sum_test_cpus[] = {1, 2, 4, 8};
const unsigned int prefix_sum_test_seeds[] = {0};
const size_t prefix_sum_num_problems[] = {1, 2, 4};
const size_t prefix_sum_test_n[] = {100000000};
const int prefix_sum_test_types[] = {0};

#define SSSP_TEST true
const procs_t sssp_test_cpus[] = {1, 2, 4, 8};
const unsigned int sssp_test_seeds[] = {0}; //, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
const GraphBipartitioningProblem sssp_test_problems[] = {
		// n, p, max_w
		{3000, 0.5, 100000000},
};
const int sssp_test_types[] = {0};
const size_t sssp_test_k[] = {1024};// {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};

//#define SSSP_SIM true
//#define SSSP_SIM_STRUCTURED true
//#define SSSP_SIM_DEP_CHECK true
//#define SSSP_SIM_VERIFY_THEORY true
const size_t sssp_sim_k = 1000;
const size_t sssp_sim_p = 200;

const double sssp_sim_theory_p = 0.1;
const double sssp_sim_theory_div = 100000000;




//#define SORANDUTS


}

#endif /* SHORT_SINGLE_H_ */
