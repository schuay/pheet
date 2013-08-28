/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "LockedSet.h"

namespace pareto
{

void
LockedSet::
insert(sp::Paths paths)
{
	m_mutex.lock();
	NaiveSet::insert(paths);
	m_mutex.unlock();
}

}
