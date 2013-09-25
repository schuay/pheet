/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STRATEGYMSPSTRATEGY_H_
#define STRATEGYMSPSTRATEGY_H_

#include <vector>

#include "lib/ShortestPath/Path.h"

namespace pheet
{

/**
 * Multi-criteria shortest path strategy for use with a priority queue scheduler.
 *
 * A total order is induced on the weights by creating a linear combination of all
 * weight elements: h(ws) = ws[0] * f[0] + ... + ws[K - 1] * f[K - s]. f is an array
 * of random double factors <- ]0., 1.] assigned statically to each place.
 */

template <class Pheet>
class StrategyMspStrategy : public Pheet::Environment::BaseStrategy
{
public:
	typedef StrategyMspStrategy<Pheet> Self;
	typedef typename Pheet::Environment::BaseStrategy BaseStrategy;
	typedef std::vector<int> Weights;

	StrategyMspStrategy(sp::PathPtr path);
	StrategyMspStrategy(Self& other);
	StrategyMspStrategy(Self&& other);

	inline bool prioritize(Self& other) const;
	inline bool forbid_call_conversion() const;
	inline bool dead_task();

	static size_t default_k;

private:
	sp::PathPtr path;
};

template <class Pheet>
size_t StrategyMspStrategy<Pheet>::default_k = 1024;

template <class Pheet>
StrategyMspStrategy<Pheet>::
StrategyMspStrategy(sp::PathPtr path)
	: path(path)
{
	this->set_k(default_k);
}

template <class Pheet>
StrategyMspStrategy<Pheet>::
StrategyMspStrategy(Self& other)
	: BaseStrategy(other),
	  path(other.path)
{
}

template <class Pheet>
StrategyMspStrategy<Pheet>::
StrategyMspStrategy(Self&& other)
	: BaseStrategy(other),
	  path(other.path)
{
}

template <class Pheet>
inline bool
StrategyMspStrategy<Pheet>::
prioritize(Self& other) const
{
	StrategyMspPlace<Pheet>& place = Pheet::template place_singleton<StrategyMspPlace<Pheet>>();

	const size_t degree = path->degree();

	const graph::weight_vector_t& this_weight = path->weight();
	const graph::weight_vector_t& that_weight = other.path->weight();

	double this_priority = 0.0;
	for (int i = 0; i < degree; i++) {
		this_priority += this_weight[i] * place.factor(i);
	}

	double that_priority = 0.0;
	for (int i = 0; i < degree; i++) {
		that_priority += that_weight[i] * place.factor(i);
	}

	return (this_priority < that_priority);
}

template <class Pheet>
inline bool
StrategyMspStrategy<Pheet>::
forbid_call_conversion() const
{
	return true;
}

template <class Pheet>
inline bool
StrategyMspStrategy<Pheet>::
dead_task()
{
	return path->dominated();
}

} /* namespace pheet */

#endif /* STRATEGYMSPSTRATEGY_H_ */
