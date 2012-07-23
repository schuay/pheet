/*
 * SimpleSssp.h
 *
 *  Created on: Jul 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef SIMPLESSSP_H_
#define SIMPLESSSP_H_

#include "../sssp_graph_helpers.h"

namespace pheet {

template <class Pheet>
class SimpleSssp : public Pheet::Task {
public:
	typedef SimpleSssp<Pheet> Self;

	SimpleSssp(SsspGraphVertex* graph, size_t size)
	:graph(graph), node(0), distance(0) {}
	SimpleSssp(SsspGraphVertex* graph, size_t node, size_t distance)
	:graph(graph), node(node), distance(distance) {}
	virtual ~SimpleSssp() {}

	virtual void operator()() {
		size_t d = graph[node].distance;
		if(d != distance) {
			// Distance has already been improved in the meantime
			// No need to check again
			return;
		}
		for(size_t i = 0; i < graph[node].num_edges; ++i) {
			size_t new_d = d + graph[node].edges[i].weight;
			size_t target = graph[node].edges[i].target;
			size_t old_d = graph[target].distance;
			while(old_d > new_d) {
				if(SIZET_CAS(&(graph[target].distance), old_d, new_d)) {
					Pheet::template
						spawn<Self>(graph, target, new_d);
					break;
				}
				old_d = graph[target].distance;
			}
		}
	}

	static char const name[];
private:
	SsspGraphVertex* graph;
	size_t node;
	size_t distance;
};

template <class Pheet>
char const SimpleSssp<Pheet>::name[] = "Simple Sssp";

} /* namespace pheet */
#endif /* SIMPLESSSP_H_ */
