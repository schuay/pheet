#ifndef LESS_H_
#define LESS_H_

#include "pheet/ds/StrategyTaskStorage/ParetoLocality/PriorityVector.h"

namespace pheet
{

/**
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
template<size_t N>
class less //TODO: we have pareto::less as well
{
public:
	bool operator()(PriorityVector<N> const*  l, PriorityVector<N> const* r) const
	{
		assert(l && r);
		for (size_t i = 0; i < N; i++) {
			if (l->at(i) > r->at(i)) {
				return false;
			}
		}
		return true;
	}
};

} /* namespace pheet */

#endif /* LESS_H_ */
