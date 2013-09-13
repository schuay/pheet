/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

//Adapted from:
//http://condor.depaul.edu/rjohnson/source/graph_ge.c

#ifndef __GENERATOR_H
#define __GENERATOR_H

#include <random>
#include "test/msp/lib/Graph/Edge.h"
#include "test/msp/lib/Graph/Graph.h"
#include "test/msp/lib/Graph/Node.h"

namespace graph
{

class Generator
{

public:
	/** generate a random connected directed graph */
	static Graph* directed(std::string const name,
	                       size_t const n,                    /**< Number of nodes (n > 0) */
	                       size_t const m,                    /**< Number of edges (n-1 <= m).
                                                                    If !allow_parallel_edges: m <= n*(n-1)  */
	                       bool const allow_parallel_edges,   /**< If true, graph may contain parallel edges */
	                       size_t const degree,
	                       size_t const
	                       weight_limit,            /**< Upper limit for the elements of a weight vector of an edge.
																	For all elements e of a weight vector hold: 0 <=  e <= weight_limit   */
	                       unsigned short const seed);         /**< seed value for random number generator */
	/** generate a random connected directed graph with
	    N = 10
	    M = 15
	    name = "g"
	    parallel edges allowed
	  */
	static Graph* basic(unsigned short const seed);

private:
	/** generate a weight vector w with d = weight_limits.size elements and
		forall i: 0 <= w[i] <= weight_limit */
	static std::vector<int>
	generate_weight_vector(std::default_random_engine& random,
	                       int const degree,
	                       int const weight_limit);
};

}

#endif /*__GENERATOR_H */
