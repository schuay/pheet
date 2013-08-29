/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STRATEGYMSPSTRATEGY_H_
#define STRATEGYMSPSTRATEGY_H_

#include <algorithm>
#include <vector>

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

	StrategyMspStrategy(const Weights& weights);
	StrategyMspStrategy(Self& other);
	StrategyMspStrategy(Self&& other);

	inline bool prioritize(Self& other) const;
	inline bool forbid_call_conversion() const;
	inline bool dead_task();

	static size_t default_k;

private:
	Weights weights;
};

template <class Pheet>
size_t StrategyMspStrategy<Pheet>::default_k = 1024;

template <class Pheet>
StrategyMspStrategy<Pheet>::
StrategyMspStrategy(const Weights& weights)
	: weights(weights)
{
	this->set_k(default_k);
}

template <class Pheet>
StrategyMspStrategy<Pheet>::
StrategyMspStrategy(Self& other)
	: BaseStrategy(other),
	  weights(other.weights)
{
}

template <class Pheet>
StrategyMspStrategy<Pheet>::
StrategyMspStrategy(Self&& other)
	: BaseStrategy(other),
	  weights(other.weights)
{
}

template <class Pheet>
inline bool
StrategyMspStrategy<Pheet>::
prioritize(Self& other) const
{
	/* TODO: The importance assigned to each weight component depends on the place.
	 * How can we access these here? */

	int lhs = 0, rhs = 0;
	std::for_each(weights.begin(), weights.end(), [&lhs](int w) {
		lhs += w;
	});
	std::for_each(other.weights.begin(), other.weights.end(), [&rhs](int w) {
		rhs += w;
	});

	return (lhs < rhs);
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
	return false; /* TODO */
}

} /* namespace pheet */

#endif /* STRATEGYMSPSTRATEGY_H_ */
