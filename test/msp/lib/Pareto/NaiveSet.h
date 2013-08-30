/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef NAIVE_SET_H
#define NAIVE_SET_H


#include <unordered_set>
#include "Set.h"

namespace pareto
{

class NaiveSet : public Set
{
public:
	virtual sp::Paths insert(sp::Paths& paths) override;
	virtual bool insert(sp::PathPtr& path) override;

private:
	typedef sp::PathPtr elem_t;
	std::unordered_set<elem_t> m_set;
};

}

#endif // NAIVE_SET_H
