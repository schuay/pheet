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
