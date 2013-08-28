#include "Generator.h"

#include <algorithm>
#include <assert.h>
#include <random>

#define N (10)
#define M (15)

using namespace graph;

Graph*
Generator::
directed(const std::string& name,
         const size_t& n,
         const size_t& m,
         const bool& allow_parallel_edges,
         const std::vector<std::pair<int, int>>& weight_limits,
         const unsigned short seed)
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

	for (auto & it : weight_limits) {
		assert(it.first <= it.second);
	}

	std::default_random_engine random(seed);
	Graph* g = new Graph(name, weight_limits.size());
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
		g->add_edge(tree[tail], tree[head], generate_weight_vector(weight_limits));
	}

	/* Add additional random edges until achieving desired number */
	for (size_t i = n - 1; i < m; i++) {
		tail = random() % n;
		do {
			head = random() % n;
		} while (head == tail || (!allow_parallel_edges && g->contains_edge(tree[tail], tree[head])));

		g->add_edge(tree[tail], tree[head], generate_weight_vector(weight_limits));
	}
	return g;
}

Graph*
Generator::
basic(const unsigned short seed)
{
	Wl wl;
	wl.push_back(std::pair<int, int>(0, 5));
	wl.push_back(std::pair<int, int>(3, 10));
	wl.push_back(std::pair<int, int>(1, 7));
	return Generator::directed("g", N, M, true, wl, seed);
}

std::vector<int>
Generator::
generate_weight_vector(const Wl& weight_limits)
{
	std::vector<int> weights;
	for (auto & it : weight_limits) {
		int w = it.first + rand() % (it.second - it.first + 1);
		weights.push_back(w);
	}
	return weights;
}

Generator::Wl
Generator::
default_weights()
{
	return { std::pair<int, int>(0, 10),
	         std::pair<int, int>(0, 10),
	         std::pair<int, int>(0, 10)
	       };

}
