/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef MSPTESTS_H_
#define MSPTESTS_H_

#include "../init.h"

namespace pheet
{

/**
 * MSP stands for the Multi-criteria (single source) Shortest Path problem.
 *
 * In short, given a graph G = (V, E), a weight vector c(e) of dimension k for each
 * edge e <- E, as well as a start node s, the algorithm returns all Pareto optimal paths
 * to all nodes reachable from s.
 */

class MspTests
{
public:
	void run_test();

private:
	template<class Pheet, template <class P> class Partitioner>
	void run_algorithm();
};

} /* namespace pheet */

#endif /* MSPTESTS_H_ */