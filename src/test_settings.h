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
