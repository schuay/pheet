//Adapted from:
//http://condor.depaul.edu/rjohnson/source/graph_ge.c

#ifndef __GENERATOR_H
#define __GENERATOR_H

#include "test/msp/lib/Graph/Edge.h"
#include "test/msp/lib/Graph/Graph.h"
#include "test/msp/lib/Graph/Node.h"

namespace graph
{

class Generator
{
public:
	typedef std::vector<std::pair<int, int>> Wl;

public:
	/** generate a random connected directed graph */
	static Graph* directed(const std::string& name,
	                       const size_t& n,                    /**< Number of nodes (n > 0) */
	                       const size_t& m,                    /**< Number of edges (n-1 <= m).
                                                                    If !allow_parallel_edges: m <= n*(n-1)  */
	                       const bool& allow_parallel_edges,   /**< If true, graph may contain parallel edges */
	                       const Wl& weight_limits,            /**< Limits [min, max] for the weight vector of an edge.
                                                                    length of vector = d = dimension of graph */
	                       const unsigned short seed);         /**< seed value for rand() */
	/** generate a random connected directed graph with
	    N = 10
	    M = 15
	    name = "g"
	    parallel edges allowed
	    weight vector limits: [(0,1),(3,10),(-2,2)]
	  */
	static Graph* basic(const unsigned short seed);

	static Wl default_weights();

private:
	/** generate a weight vector w with d = weight_limits.size elements and
	    forall i: weight_limits[i].first <= w[i] <= weight_limits[i].second */
	static std::vector<int> generate_weight_vector(const Wl& weight_limits);
};

}

#endif /*__GENERATOR_H */
