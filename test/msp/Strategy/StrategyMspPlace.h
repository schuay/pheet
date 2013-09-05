/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STRATEGYMSPPLACE_H_
#define STRATEGYMSPPLACE_H_

#include <random>
#include <vector>

namespace pheet
{

/**
 * Each place is assigned a collection of random positive double factors,
 * which are used to generate the (place-specific) priority in Strategy::prioritize().
 * We don't know in advance how many factors we need, so we need to use lazy generation.
 */
template <class Pheet>
class StrategyMspPlace
{
public:
	StrategyMspPlace();

	double
	factor(int n);

private:
	std::vector<double> factors;
	std::default_random_engine gen;
};

template <class Pheet>
StrategyMspPlace<Pheet>::
StrategyMspPlace()
	: gen(Pheet::get_place_id())
{
}

template <class Pheet>
double
StrategyMspPlace<Pheet>::
factor(int n)
{
	/* TODO: Ensure no factor is equal to 0. */

	while (n + 1 > factors.size()) {
		factors.push_back(gen() / (double)gen.max());
	}

	return factors[n];
}

}

#endif
