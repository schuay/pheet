/*
 * settings.h
 *
 *  Created on: 28.03.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef PHEET_SETTINGS_H_
#define PHEET_SETTINGS_H_

#include "misc/types.h"

//#define NDEBUG

const pheet::procs_t system_max_cpus = 4;

#define ENV_LINUX_GCC	1
//#define ENV_LINUX_ICC	1
//#define_ENV_SOLARIS_SUNCC	1
//#define ENV_CYGWIN_GCC	1

#define ENV_X86		1
//#define ENV_SPARC	1

#ifdef ENV_LINUX_GCC
#define thread_local __thread
#elif ENV_CYGWIN_GCC
#define thread_local __thread
#endif

#include "performance_settings.h"

#endif /* PHEET_SETTINGS_H_ */
