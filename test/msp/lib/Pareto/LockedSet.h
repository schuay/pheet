/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

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
	virtual sp::Paths insert(sp::Paths& paths) override;
	virtual bool insert(sp::PathPtr& path) override;

private:
	std::mutex m_mutex;

};
}

#endif /* LOCKED_SET_H */
