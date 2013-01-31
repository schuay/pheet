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
#include "SimpleSsspPerformanceCounters.h"

namespace pheet {

template <class Pheet>
class SimpleSssp : public Pheet::Task {
public:
	typedef SimpleSssp<Pheet> Self;
	typedef SimpleSsspPerformanceCounters<Pheet> PerformanceCounters;

	SimpleSssp(SsspGraphVertex* graph, size_t size, PerformanceCounters& pc)
	:graph(graph), node(0), distance(0), pc(pc) {}
	SimpleSssp(SsspGraphVertex* graph, size_t node, size_t distance, PerformanceCounters& pc)
	:graph(graph), node(node), distance(distance), pc(pc) {}
	virtual ~SimpleSssp() {}

	virtual void operator()() {
		size_t d = graph[node].distance;
		if(d != distance) {
			pc.num_dead_tasks.incr();
			// Distance has already been improved in the meantime
			// No need to check again
			return;
		}
		pc.num_actual_tasks.incr();
		for(size_t i = 0; i < graph[node].num_edges; ++i) {
			size_t new_d = d + graph[node].edges[i].weight;
			size_t target = graph[node].edges[i].target;
			size_t old_d = graph[target].distance;
			while(old_d > new_d) {
				if(SIZET_CAS(&(graph[target].distance), old_d, new_d)) {
					Pheet::template
						spawn<Self>(graph, target, new_d, pc);
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
	PerformanceCounters pc;
};

template <class Pheet>
char const SimpleSssp<Pheet>::name[] = "Simple Sssp";

} /* namespace pheet */
#endif /* SIMPLESSSP_H_ */
