/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __LOCKED_SET_H
#define __LOCKED_SET_H

#include <mutex>

#include "Set.h"

namespace pareto
{

/**
 * Wrapper for a basic set to provide sychronized access for multiple processors
 */
template<class T>
class LockedSet : public Set
{
public:

	LockedSet();

	LockedSet(sp::PathPtr& init);

	void insert(sp::PathPtr& path,
	            sp::Paths& added,
	            sp::Paths& removed) override;

	sp::Paths paths() const override;

	~LockedSet();

private:
	T* m_set;
	std::mutex m_mutex;
};

}

#endif /* __LOCKED_SET_H */
