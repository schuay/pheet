/*
 * test_settings.h
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef TEST_SETTINGS_H_
#define TEST_SETTINGS_H_

#include "test_variants/full.h"

namespace pheet {

#ifdef LUPIV_TEST
const bool    lupiv_test        = LUPIV_TEST;
#else
const bool    lupiv_test        = false;
#endif

}

#endif /* TEST_SETTINGS_H_ */
