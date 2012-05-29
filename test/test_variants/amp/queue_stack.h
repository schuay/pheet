/*
 * queue_stack.h
 *
 *  Created on: May 29, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef QUEUE_STACK_H_
#define QUEUE_STACK_H_

namespace pheet {

#define AMP_QUEUE_STACK_TEST true

#define SORTING_TEST true
const procs_t sorting_test_cpus[] = {1, 2, 3, 6, 12, 24, 48, 96};
const unsigned int sorting_test_seeds[] = {0, 1};
const size_t sorting_test_n[] = {1000000, 10000000};
const int sorting_test_types[] = {0, 1};


const bool    nqueens_test        = false;
const procs_t nqueens_test_cpus[] = {1, 2, 4, 8, 16, 32, 64, 128};
const size_t  nqueens_test_n[]    = {24, 32};
}

#endif /* QUEUE_STACK_H_ */
