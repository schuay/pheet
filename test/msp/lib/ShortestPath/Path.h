/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __PATH_H
#define __PATH_H

#include <atomic>
#include <memory>
#include <vector>

#include "test/msp/lib/Graph/Edge.h"

namespace sp
{

constexpr int BLOCK_SIZE = 4096;

class Path;
typedef std::shared_ptr<Path> PathPtr;
typedef std::vector<PathPtr> Paths;

class Path
{
public:
	Path();
	Path(Path const& that);
	Path(graph::Node const* init);

	/** Sets up the current path object to represent the path generated
	 *  by appending the given edge to the given path. */
	void step(graph::Edge const* edge, PathPtr const path);

	void print() const;
	void print_verbose() const;

	graph::Node const* tail() const;
	graph::Node const* head() const;

	inline graph::weight_vector_t const& weight() const {
		return m_weight;
	}

	inline graph::weight_t weight_sum() const {
		return m_weight_sum;
	}

	inline size_t degree() const {
		return m_degree;
	}

	bool dominated() const;
	void set_dominated();

private:
	graph::Node const* m_tail;
	graph::Node const* m_head;
	Path const* m_pred;
	std::vector<graph::weight_t> m_weight;
	graph::weight_t m_weight_sum;
	size_t m_degree;
	std::atomic_bool m_dominated;
};

class PathReuseCheck
{
public:
	inline bool operator()(Path const& path) const {
		return false; //path.unused();
	}
};

}

#endif /* __PATH_H */
