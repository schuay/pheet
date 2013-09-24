/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "PaixaoSantos.h"

#include <assert.h>
#include <limits>

using std::string;

namespace graph
{

static const string C_COMMENT = "c";
static const string C_TYPE    = "T";
static const string C_NODES   = "N";
static const string C_EDGES   = "A";
static const string C_DEGREE  = "k";
static const string C_START   = "s";
static const string C_END     = "t";
static const string C_EDGE    = "a";

enum {
    S_PROPERTIES,
    S_EDGES
};

static void
parse_edge(std::istream& in, Graph* g)
{
	int tail = -1;
	int head = -1;

	in >> tail >> head;

	assert(tail > -1 && (size_t)tail <= g->node_count());
	assert(head > -1 && (size_t)head <= g->node_count());

	weight_vector_t ws;
	ws.reserve(g->degree());

	for (size_t i = 0; i < g->degree(); i++) {
		weight_t w;
		in >> w;
		ws.push_back(w);
	}

	Node* t = g->get_node(tail - 1);
	Node* h = g->get_node(head - 1);

	g->add_edge(t, h, ws);
}

static void
skip_line(std::istream& in)
{
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

Graph*
PaixaoSantos::
operator()(std::istream& in) const
{
	Graph* g = nullptr;

	int state = S_PROPERTIES;

	int nodes  = -1;
	int edges  = -1;
	int degree = -1;
	int start  = -1;
	int end    = -1;

	string line_type, network_structure, arc_orientation;

	while (in >> line_type) {
		switch (state) {
		case S_PROPERTIES:
			if (line_type == C_TYPE) {
				in >> network_structure >> arc_orientation;
			} else if (line_type == C_NODES) {
				in >> nodes;
			} else if (line_type == C_EDGES) {
				in >> edges;
			} else if (line_type == C_DEGREE) {
				in >> degree;
			} else if (line_type == C_START) {
				in >> start;
			} else if (line_type == C_END) {
				in >> end;
			} else if (line_type == C_EDGE) {
				assert(nodes  != -1);
				assert(edges  != -1);
				assert(degree != -1);
				assert(start  != -1);
				assert(end    != -1);

				assert(!network_structure.empty());
				assert(!arc_orientation.empty());

				state = S_EDGES;

				string name = "Paixao-Santos: " + network_structure + " " + arc_orientation;
				g = new Graph(name, degree);

				for (int i = 0; i < nodes; i++) {
					g->add_node();
				}

				parse_edge(in, g);
			} else if (line_type == C_COMMENT) {
				skip_line(in);
			} else {
				assert(0);
			}
			break;
		case S_EDGES:
			if (line_type == C_EDGE) {
				parse_edge(in, g);
			} else if (line_type == C_COMMENT) {
				skip_line(in);
			} else {
				assert(0);
			}
			break;
		default:
			assert(0);
			break;
		}
	}

	return g;
}

}
