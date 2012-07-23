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

namespace pheet {

template <class Pheet>
class StrategySssp : public Pheet::Task {
public:
	typedef StrategySssp<Pheet> Self;
	typedef StrategySsspStrategy<Pheet> Strategy;

	StrategySssp(SsspGraphVertex* graph, size_t size)
	:graph(graph), node(0), distance(0) {}
	StrategySssp(SsspGraphVertex* graph, size_t node, size_t distance)
	:graph(graph), node(node), distance(distance) {}
	virtual ~StrategySssp() {}

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
						spawn_s<Self>(
								Strategy(new_d),
								graph, target, new_d);
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
char const StrategySssp<Pheet>::name[] = "Strategy Sssp";

} /* namespace pheet */
#endif /* STRATEGYSSSP_H_ */
