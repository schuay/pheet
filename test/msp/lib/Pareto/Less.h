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
	bool operator()(const sp::Path* lhs,
	                const sp::Path* rhs) const;
};

}

#endif /* __LESS_H_ */
