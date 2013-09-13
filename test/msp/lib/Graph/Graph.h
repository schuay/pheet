/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __GRAPH_H
#define __GRAPH_H

#include <string>
#include <unordered_map>
#include <vector>

/* Forward declarations copied from cgraph.h. */

typedef struct Agraph_s Agraph_t;
typedef struct Agnode_s Agnode_t;
typedef struct Agedge_s Agedge_t;

namespace graph
{

class Node;
class Edge;

typedef int weight_t;
typedef std::vector<weight_t> weight_vector_t;

#define ATTR_WEIGHT const_cast<char *>("label")
#define ATTR_DEGREE const_cast<char *>("degree")
#define ATTR_NODEID const_cast<char *>("label")

class Graph
{
	friend class Node;
	friend class Edge;

public:
	Graph(std::string const& name,
	      size_t const degree);
	virtual ~Graph();

	static Graph* read(FILE* f);
	bool write(FILE* f) const;

	size_t degree() const;
	std::string name() const;
	bool contains_edge(Node const* tail,
	                   Node const* head) const;

	Node* add_node();
	Edge* add_edge(Node* tail,
	               Node* head,
	               weight_vector_t const& weights);

	size_t node_count() const;
	size_t edge_count() const;

	Node* get_node(ulong const id) const;
	std::vector<Node*> nodes() const;

private:
	Graph(Agraph_t* g);

	Edge* get_edge(ulong const id) const;

	void add_node(ulong const id,
	              Node* n);
	void add_edge(ulong const id,
	              Edge* e);

private:
	Agraph_t* g;

	size_t m_degree;

	std::unordered_map<ulong, Node*> m_nodes;
	std::unordered_map<ulong, Edge*> m_edges;
};

}

#endif /* __GRAPH_H */
