/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Less.h"

#include <assert.h>

using namespace graph;
using namespace sp;

using std::vector;

namespace pareto
{

bool
less::
operator()(Path const* lhs,
           Path const* rhs) const
{
	assert(lhs && rhs);

	weight_vector_t const& l = lhs->weight();
	weight_vector_t const& r = rhs->weight();

	const int dims = lhs->degree();
	assert(dims == rhs->degree());

	for (int i = 0; i < dims; i++) {
		if (l[i] > r[i]) {
			return false;
		}
	}

	return true;
}

}
