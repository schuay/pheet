/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "KDTree.h"

#include "assert.h"

using namespace graph;
using namespace sp;

namespace pareto
{

/**
 * The left branch is <=, the right is >.
 */
struct tree_t {
	const PathPtr p;
	tree_t* l;
	tree_t* r;
	size_t height;
	bool active;
};

KDTree::
KDTree() :
	t(nullptr),
	imba(0),
	dominates()
{
}

static void
tree_delete(tree_t* t)
{
	if (!t) {
		return;
	}

	tree_delete(t->l);
	tree_delete(t->r);

	delete t;
}

KDTree::
~KDTree()
{
	tree_delete(t);
}

bool
KDTree::
dominated(tree_t const* t,
          const size_t i,
          const PathPtr path) const
{
	if (!t) {
		return false;
	}

	if (t->active && dominates(t->p.get(), path.get())) {
		return true;
	}

	const size_t j = (i + 1) % path->degree();

	if (t->p->weight()[i] <= path->weight()[i]) {
		if (dominated(t->r, j, path)) {
			return true;
		}
	}

	return dominated(t->l, j, path);
}


bool
KDTree::
dominated(const PathPtr path) const
{
	return dominated(t, 0, path);
}

void
KDTree::
prune(tree_t* t,
      const size_t i,
      const PathPtr path,
      Paths& pruned)
{
	if (!t) {
		return;
	}

	if (t->active && dominates(path.get(), t->p.get())) {
		t->active = false;
		pruned.push_back(t->p);
	}

	const size_t j = (i + 1) % path->degree();

	if (t->p->weight()[i] >= path->weight()[i]) {
		prune(t->l, j, path, pruned);
	}

	prune(t->r, j, path, pruned);
}

void
KDTree::
prune(const PathPtr path,
      Paths& pruned)
{
	prune(t, 0, path, pruned);
}

static int
tree_height(tree_t const* t)
{
	return (t == nullptr) ? -1 : t->height;
}

void
KDTree::
insert(const PathPtr path)
{
	assert(path);
	insert(&t, 0, path);
}

void
KDTree::
insert(tree_t** t,
	   const size_t i,
	   const PathPtr path)
{
	weight_vector_t const& ws = path->weight();
	const size_t degree = path->degree();

	if (*t == nullptr) {
		*t = new tree_t { path, nullptr, nullptr, 0, true };
		return;
	}

	tree_t* tt = *t;
	insert((ws[i] <= tt->p->weight()[i]) ? &tt->l : &tt->r,
		   (i + 1) % degree, path);

	tt->height++;
	imba = std::max(imba, std::abs(tree_height(tt->l) - tree_height(tt->r)));
}

void
KDTree::
items(const tree_t* t, Paths& paths) const
{
	if (!t) {
		return;
	}

	assert(t->p);

	if (t->active) {
		paths.push_back(t->p);
	}

	items(t->l, paths);
	items(t->r, paths);
}

Paths
KDTree::
items() const
{
	Paths ps;
	items(t, ps);
	return ps;
}

}
