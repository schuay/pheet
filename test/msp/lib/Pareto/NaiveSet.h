/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __NAIVE_SET_H
#define __NAIVE_SET_H

#include <unordered_set>

#include "Set.h"

namespace pareto
{

class NaiveSet : public Set
{
public:

	NaiveSet();

	NaiveSet(sp::PathPtr& init);

	void insert(sp::PathPtr& path,
	            sp::Paths& added,
	            sp::Paths& removed) override;

	sp::Paths paths() const override;

private:
	std::unordered_set<sp::PathPtr> m_set;
};

}

#endif // __NAIVE_SET_H
