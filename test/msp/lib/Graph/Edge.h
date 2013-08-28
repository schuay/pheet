#ifndef __EDGE_H
#define __EDGE_H

#include <vector>

#include "Node.h"

namespace graph
{

class Edge
{
	friend class Graph;
public:
	Node* tail() const;
	Node* head() const;

	graph::weight_vector_t weights() const;

private:
	Edge(Node* t,
	     Node* h,
	     const graph::weight_vector_t& ws);
	Edge(Graph* graph,
	     Agedge_t* e);

	Graph* graph() const;

private:
	Graph* m_graph;
	Node* t, *h;
	Agedge_t* e;

	graph::weight_vector_t ws;
};

}

#endif /* __EDGE_H */
