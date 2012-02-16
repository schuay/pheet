/*
 * assert.h
 *
 *  Created on: Feb 15, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PHEET_ASSERT_H_
#define PHEET_ASSERT_H_

#include <assert.h>
#ifdef PHEET_DEBUG_MODE
#define pheet_assert(expr)	assert(expr)
#else
#define pheet_assert(expr)		(static_cast<void> (0))
#endif

#endif /* ASSERT_H_ */