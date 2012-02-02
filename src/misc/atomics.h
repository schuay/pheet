/*
 * atomics.h
 *
 *  Created on: 28.03.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef ATOMICS_H_
#define ATOMICS_H_

#include "../settings.h"

#ifdef ENV_LINUX_GCC

#include "atomics_gnu.h"

#elif ENV_LINUX_CLANG

#include "atomics_clang.h"

#elif ENV_LINUX_ICC

#include "atomics_gnu.h"

#elif ENV_CYGWIN_GCC

#include "atomics_gnu.h"

#elif ENV_SOLARIS_SUNCC

#include "atomics_sun.h"

#endif

#endif /* ATOMICS_H_ */
