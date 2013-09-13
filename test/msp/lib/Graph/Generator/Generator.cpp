/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Generator.h"

#include <algorithm>
#include <assert.h>

#define N (10)
#define M (15)
#define W (100000)

using namespace graph;

Graph*
Generator::
directed(std::string const name,
         size_t const n,
         size_t const m,
         bool const allow_parallel_edges,
         size_t const degree,
         size_t const weight_limit,
         unsigned short const seed)
{
	assert(n > 0);
	assert(m >= n - 1);

	//can't add any edges if g has less then 2 nodes
	if (n < 2) {
		assert(m == 0);
	}

	if (!allow_parallel_edges) {
		assert(m <= n * (n - 1));
	}


	std::default_random_engine random(seed);
	Graph* g = new Graph(name, degree);
	std::vector<Node*> tree;

	/*  Generate a random permutation in the array tree. */
	for (size_t i = 0; i < n; i++) {
		tree.push_back(g->add_node());
	}
	std::shuffle(tree.begin(), tree.end(), random);

	/*  Next generate a random spanning tree.
	    The algorithm is:

	      Assume that vertices tree[ 0 ],...,tree[ i - 1 ] are in
	      the tree.  Add an edge incident on tree[ i ]
	      and a random vertex in the set {tree[ 0 ],...,tree[ i - 1 ]}.
	 */

	size_t tail, head;

	for (head = 1; head < n; head++) {
		tail = random() % head;
		g->add_edge(tree[tail], tree[head], generate_weight_vector(random, degree, weight_limit));
	}

	/* Add additional random edges until achieving desired number */
	for (size_t i = n - 1; i < m; i++) {
		tail = random() % n;
		do {
			head = random() % n;
		} while (head == tail || (!allow_parallel_edges && g->contains_edge(tree[tail], tree[head])));

		g->add_edge(tree[tail], tree[head], generate_weight_vector(random, degree, weight_limit));
	}
	return g;
}

Graph*
Generator::
basic(unsigned short const seed)
{
	return Generator::directed("g", N, M, true, 3, 100, seed);
}

std::vector<int>
Generator::
generate_weight_vector(std::default_random_engine& random,
                       int const degree,
                       int const weight_limit)
{
	std::vector<int> weights;
	for (int i = 0; i < degree; i++) {
		int w = random() % weight_limit;
		weights.push_back(w);
	}
	return weights;
}
