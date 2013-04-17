/*
 * SsspAnalysis.h
 *
 *  Created on: Jul 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef SSSPANALYSIS_H_
#define SSSPANALYSIS_H_

#ifdef SSSP_SIM

#include "SsspAnalysisPerformanceCounters.h"

#include <vector>

//#include <pheet/ds/PriorityQueue/Merge/MergeHeap.h>

namespace pheet {

struct SsspAnalysisNode {
	size_t node_id;
	size_t distance;
	size_t added;
	bool processed;
};

struct SsspAnalysisNodeLess {
	bool operator()(SsspAnalysisNode const& n1, SsspAnalysisNode const& n2) {
		return n1.distance < n2.distance;
	}
};

template <class Pheet>
class SsspAnalysis : public Pheet::Task {
public:
	typedef SsspAnalysisPerformanceCounters<Pheet> PerformanceCounters;

	SsspAnalysis(SsspGraphVertex* graph, size_t size, PerformanceCounters& pc)
	:graph(graph), size(size), pc(pc) {}
	virtual ~SsspAnalysis() {}

	virtual void operator()() {
		SsspAnalysisNodeLess less;

		size_t const k = sssp_sim_k;
		size_t const block_size = sssp_sim_p;
		size_t phase = 0;

		SsspAnalysisNode n;
		n.distance = 0;
		n.node_id = 0;
		n.added = 0;
		n.processed = false;

		size_t* settled = new size_t[size];
		settled[0] = 0;

		graph[0].distance = 0;

		std::vector<SsspAnalysisNode> v;
		v.push_back(n);
		size_t offset = 0;

		while(v.size() > offset) {
			n = v[offset];
			if(n.processed) {
				++offset;
				continue;
			}
			size_t node = n.node_id;

			size_t d = graph[node].distance;
			if(d != n.distance) {
				++offset;
				pc.num_dead_tasks.incr();
				// Distance has already been improved in the meantime
				// No need to check again
				continue;
			}
			++phase;

			std::cout << "Phase " << phase << ":" << std::endl;

			size_t sum = 0;
			size_t samples = 0;
			size_t base = v[offset].distance;
			for(size_t i = offset; i < v.size(); ++i) {
				if(graph[v[i].node_id].distance == v[i].distance && !v[i].processed) {
					++samples;
					sum += v[i].distance - base;
					std::cout << v[i].distance/* - base*/;
					if(v[i].added >= v.size() - k) {
						std::cout << "*";
					}
					std::cout << ",";
				}
				else {
					pheet_assert(i != offset);
				}
			}
			if(samples > 0) {
				std::cout << std::endl << (sum / (samples)) << std::endl;
			}
			else {
				std::cout << std::endl;
			}

			size_t orig_size = v.size();
			size_t sum_new = 0;
			size_t sum_upd = 0;
			size_t j, j2;
			for(j = 0, j2 = 0; j < block_size && offset + j2 < orig_size; ++j2) {
				n = v[offset + j2];
				size_t node = n.node_id;
				size_t d = graph[node].distance;
				size_t a = n.added;

				if(j == 0 || a < orig_size - k) {
					if(d == n.distance && !n.processed) {
						pc.num_actual_tasks.incr();
						for(size_t i = 0; i < graph[node].num_edges; ++i) {
							size_t new_d = d + graph[node].edges[i].weight;
							size_t target = graph[node].edges[i].target;
							size_t old_d = graph[target].distance;
							if(old_d > new_d) {
								if(old_d == std::numeric_limits<size_t>::max()) {
									++sum_new;
								}
								else {
									++sum_upd;
								}
								graph[target].distance = new_d;
								n.distance = new_d;
								n.node_id = target;
						//		n.added = v.size();
								n.processed = false;
								v.push_back(n);
							}
						}
						v[offset + j2].processed = true;
						settled[node] = phase;
						++j;
					}
				}
//				++offset;
			}
			if(orig_size - offset > 1 && j < block_size) {
				std::random_shuffle(v.begin() + offset + 1, v.begin() + orig_size);

				for(j2 = 1;j < block_size && offset + j2 < orig_size; ++j2) {
					n = v[offset + j2];
					size_t node = n.node_id;
					size_t d = graph[node].distance;
					size_t a = n.added;

					if(j == 0 || a >= orig_size - k) {
						if(d == n.distance && !n.processed) {
							pc.num_actual_tasks.incr();
							for(size_t i = 0; i < graph[node].num_edges; ++i) {
								size_t new_d = d + graph[node].edges[i].weight;
								size_t target = graph[node].edges[i].target;
								size_t old_d = graph[target].distance;
								if(old_d > new_d) {
									if(old_d == std::numeric_limits<size_t>::max()) {
										++sum_new;
									}
									else {
										++sum_upd;
									}
									graph[target].distance = new_d;
									n.distance = new_d;
									n.node_id = target;
							//		n.added = v.size();
									n.processed = false;
									v.push_back(n);
								}
							}
							v[offset + j2].processed = true;
							settled[node] = phase;
							++j;
						}
					}
				}
			}
			std::random_shuffle(v.begin() + orig_size, v.end());

			for(size_t i2 = orig_size; i2 < v.size(); ++i2) {
				v[i2].added = i2;
			}
			++offset;
			std::cout << sum_new << " - " << sum_upd << " " << v.size() << std::endl;
			std::sort(v.begin() + offset, v.end(), less);
		}

		for(size_t i = 0; i <= phase; ++i) {
			size_t counter = 0;
			for(size_t j = 0; j < size; ++j) {
				if(settled[j] == i) {
					++counter;
				}
			}
			std::cout << "Settled in phase " << i << ": " << counter << std::endl;
		}
		delete[] settled;
	}

	static char const name[];
private:
	SsspGraphVertex* graph;
	size_t size;
	PerformanceCounters pc;
};

template <class Pheet>
char const SsspAnalysis<Pheet>::name[] = "Sssp Analysis";

} /* namespace pheet */

#endif

#endif /* SSSPANALYSIS_H_ */
