/*
 * test_settings.h
 *
 *  Created on: 07.04.2011
 *      Author: mwimmer
 */

#ifndef TEST_SETTINGS_H_
#define TEST_SETTINGS_H_

#include "src/misc/types.h"


const bool sorting_test = true;
const procs_t sorting_test_cpus[] = {1, 2, 4, 8, 16, 32};
const unsigned int sorting_test_seeds[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const size_t sorting_test_n[] = {10000000, 100000000, 1000000000, 8388607, 33554431, 134217727};
const int sorting_test_types[] = {0, 1, 3, 4};

#endif /* TEST_SETTINGS_H_ */
