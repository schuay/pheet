#ifndef __LOCKED_SET_H
#define __LOCKED_SET_H

#include <mutex>
#include "NaiveSet.h"

namespace pareto
{
/**
 * Wrapper for NaiveSet to provide sychronized access for multiple processors
 */
class LockedSet : public NaiveSet
{
public:
	void insert(sp::Paths paths) override;

private:
	std::mutex m_mutex;

};
}

#endif /* LOCKED_SET_H */
