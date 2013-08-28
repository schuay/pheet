#include "Less.h"

#include <assert.h>

using namespace graph;
using namespace sp;

using std::vector;

namespace pareto
{

bool
less::
operator()(const Path* lhs,
           const Path* rhs) const
{
	assert(lhs && rhs);

	const vector<graph::weight_t> l = lhs->weight();
	const vector<graph::weight_t> r = rhs->weight();

	assert(l.size() == r.size());
	const int dims = l.size();

	for (int i = 0; i < dims; i++) {
		if (l[i] > r[i]) {
			return false;
		}
	}

	return true;
}

}
