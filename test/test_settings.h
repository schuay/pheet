/*
 * test_settings.h
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef TEST_SETTINGS_H_
#define TEST_SETTINGS_H_

namespace pheet {
struct GraphBipartitioningProblem {
	size_t n;
	double p;
	size_t max_w;
};

}

#include "test_variants/full.h"

namespace pheet {

#ifdef SORTING_TEST
const bool    sorting_test        = SORTING_TEST;
#else
const bool    sorting_test        = false;
#endif

#ifdef GRAPH_BIPARTITIONING_TEST
const bool    graph_bipartitioning_test        = SORTING_TEST;
#else
const bool    graph_bipartitioning_test        = false;
#endif

#ifdef INAROW_TEST
const bool    inarow_test        = INAROW_TEST;
#else
const bool    inarow_test        = false;
#endif

#ifdef LUPIV_TEST
const bool    lupiv_test        = LUPIV_TEST;
#else
const bool    lupiv_test        = false;
#endif

#ifdef SOR_TEST
const bool    sor_test        = SOR_TEST;
#else
const bool    sor_test        = false;
#endif

#ifdef UTS_TEST
const bool    uts_test        = UTS_TEST;
#else
const bool    uts_test        = false;
#endif

}

#endif /* TEST_SETTINGS_H_ */
