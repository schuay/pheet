/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __SORTED_SET_H
#define __SORTED_SET_H

#include <set>

#include "Less.h"
#include "Set.h"

namespace pareto
{

/**
 * This set variant is based on the idea that all items are stored
 * sorted by he sum of their weights. Dominators may only be found to the left
 * of the point at which a new path would be inserted, and are likely to appear
 * farther left within this range than right. Dominatees may only be found to the
 * right.
 */
class SortedSet : public Set
{
public:
	class lincomb_less
	{
	public:
		bool operator()(sp::PathPtr const& l,
		                sp::PathPtr const& r) const;
	};

public:
	void insert(sp::PathPtr& path,
	            sp::Paths& added,
	            sp::Paths& removed) override;

	sp::Paths paths() const override;

private:
	less dominates;
	std::multiset<sp::PathPtr, lincomb_less> m_set;
};

}

#endif // __SORTED_SET_H
