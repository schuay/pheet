/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __LESS_H_
#define __LESS_H_

#include "test/msp/lib/ShortestPath/Path.h"

namespace pareto
{

/**
 * Let l, r be the weights of lhs and rhs.
 *
 * The pareto less operator returns true iff:
 * * l dominates r, or
 * * l equals r.
 *
 * less({0, 1}, {1, 1}) -> true.
 * less({1, 1}, {1, 1}) -> true.
 *
 * less({1, 1}, {0, 1}) -> false.
 *
 * less({0, 1}, {1, 0}) -> false.
 * less({1, 0}, {0, 1}) -> false.
 */
class less
{
public:
	bool operator()(sp::Path const*  lhs,
	                sp::Path const* rhs) const;
};

}

#endif /* __LESS_H_ */
