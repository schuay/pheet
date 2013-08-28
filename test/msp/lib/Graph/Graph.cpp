/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Graph.h"

#include <graphviz/cgraph.h>
#include <stdexcept>
#include <stdio.h>

#include "Edge.h"
#include "Node.h"

namespace graph
{

Graph::
Graph(const std::string& name,
      const size_t degree)
	: m_degree(degree)
{
	g = agopen(const_cast<char*>(name.c_str()), Agdirected, NULL);

	agattr(g, AGRAPH, ATTR_DEGREE, const_cast<char*>(""));
	agattr(g, AGNODE, ATTR_NODEID, const_cast<char*>(""));
	agattr(g, AGEDGE, ATTR_WEIGHT, const_cast<char*>(""));

	agset(g, ATTR_DEGREE, const_cast<char*>(std::to_string(degree).c_str()));
}

Graph::
Graph(Agraph_t* g)
	: g(g)
{
	m_degree = std::stoi(agget(g, ATTR_DEGREE));

	for (Agnode_t* n = agfstnode(g); n != nullptr; n = agnxtnode(g, n)) {
		new Node(this, n);
	}

	for (auto & p : m_nodes) {
		Agnode_t* n = p.second->n;
		for (Agedge_t* e = agfstout(g, n); e != nullptr; e = agnxtout(g, e)) {
			new Edge(this, e);
		}
	}
}

Graph::
~Graph()
{
	for (auto & n : m_nodes) {
		delete n.second;
	}

	for (auto & e : m_edges) {
		delete e.second;
	}

	agclose(g);
}

Graph*
Graph::
read(FILE* f)
{
	Agraph_t* g = agread(f, NULL);
	return new Graph(g);
}

bool
Graph::
write(FILE* f)
{
	return (agwrite(g, f) > 0);
}

size_t
Graph::
degree() const
{
	return m_degree;
}

std::string
Graph::
name() const
{
	std::string name(agnameof(g));
	return name;
}

bool
Graph::
contains_edge(const Node* tail,
              const Node* head) const
{
	Agedge_t* e = agedge(g, tail->n, head->n, NULL, FALSE);
	return (e != nullptr);
}

Node*
Graph::
add_node()
{
	Node* n = new Node(this);
	return n;
}

Edge*
Graph::
add_edge(Node* tail,
         Node* head,
         const weight_vector_t& weights)
{
	Edge* e = new Edge(tail, head, weights);
	return e;
}

size_t
Graph::
node_count() const
{
	return agnnodes(g);
}

size_t
Graph::
edge_count() const
{
	return agnedges(g);
}


Node*
Graph::
get_node(const ulong id) const
{
	try {
		return m_nodes.at(id);
	} catch (const std::out_of_range&) {
		return nullptr;
	}
}

std::vector<Node*>
Graph::
nodes() const
{
	std::vector<Node*> ns;
	for (auto & p : m_nodes) {
		ns.push_back(p.second);
	}
	return ns;
}

Edge*
Graph::
get_edge(const ulong id) const
{
	try {
		return m_edges.at(id);
	} catch (const std::out_of_range&) {
		return nullptr;
	}
}

void
Graph::
add_node(const ulong id,
         Node* n)
{
	m_nodes.emplace(id, n);
}

void
Graph::
add_edge(const ulong id,
         Edge* e)
{
	m_edges.emplace(id, e);
}

}
