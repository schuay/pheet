/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef KDTREE_H_
#define KDTREE_H_

#include "lib/Pareto/Less.h"
#include "lib/ShortestPath/Path.h"

namespace pareto
{

struct tree_t;

class KDTree
{
public:
	KDTree();
	virtual ~KDTree();

	/**
	 * Worst case: O(n). All nodes are potential dominator candidates
	 * but none of them actually dominate path.
	 * Best case: O(1). There are no potential dominator candidates, or
	 * the first inspected node dominates path.
	 */
	bool dominated(const sp::PathPtr path) const;

	/**
	 * Worst case: O(n), all nodes need to be pruned or all nodes are
	 * potential pruning candidates.
	 * Best case: O(1). No potential pruning candidates.
	 */
	void prune(const sp::PathPtr path,
	           sp::Paths& pruned);

	/**
	 * O(log n) in a fully balanced tree. For now, we won't do balancing
	 * though, so again the worst case is O(n).
	 */
	void insert(const sp::PathPtr path);

	sp::Paths items() const;

private:
	bool dominated(tree_t const* t,
	               const size_t i,
	               const sp::PathPtr path) const;

	void prune(tree_t* t,
	           const size_t i,
	           const sp::PathPtr path,
	           sp::Paths& pruned);

	void insert(tree_t** t,
				const size_t i,
				const sp::PathPtr path);

	void items(tree_t const* t,
			   sp::Paths& paths) const;

private:
	tree_t* t;
	int imba;

	const less dominates;
};

}

#endif /* KDTREE_H_ */
