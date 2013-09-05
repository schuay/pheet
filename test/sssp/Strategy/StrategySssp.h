/*
 * StrategySssp.h
 *
 *  Created on: Jul 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef STRATEGYSSSP_H_
#define STRATEGYSSSP_H_

#include "StrategySsspStrategy.h"
#include "StrategySsspPerformanceCounters.h"

namespace pheet {

template <class Pheet>
class StrategySssp : public Pheet::Task {
public:
	typedef StrategySssp<Pheet> Self;
	typedef StrategySsspStrategy<Pheet> Strategy;
	typedef StrategySsspPerformanceCounters<Pheet> PerformanceCounters;

	StrategySssp(SsspGraphVertex* graph, size_t, PerformanceCounters& pc)
	:graph(graph), node(0), distance(0), pc(pc) {}
	StrategySssp(SsspGraphVertex* graph, size_t node, size_t distance, PerformanceCounters& pc)
	:graph(graph), node(node), distance(distance), pc(pc) {}
	virtual ~StrategySssp() {}

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
						spawn_s<Self>(
								Strategy(new_d, graph[target].distance),
								graph, target, new_d, pc);
					break;
				}
				old_d = graph[target].distance;
			}
		}
	}

	static void set_k(size_t k) {
		Strategy::default_k = k;
	}

	static char const name[];
private:
	SsspGraphVertex* graph;
	size_t node;
	size_t distance;
	PerformanceCounters pc;
};

template <class Pheet>
char const StrategySssp<Pheet>::name[] = "Strategy Sssp";

} /* namespace pheet */
#endif /* STRATEGYSSSP_H_ */
