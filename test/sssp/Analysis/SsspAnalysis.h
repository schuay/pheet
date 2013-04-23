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

		// k tasks may not be visible to others
		size_t const k = sssp_sim_k;
		// Number of items processed in each phase (= #threads in model)
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
				// Node was already processed
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
			std::cout << "Nodes in list (* ... nodes not visible to all threads) " << std::endl;

			size_t sum = 0;
			size_t processed_sum = 0;
			size_t samples = 0;
			size_t processed_samples = 0;
			size_t base = v[offset].distance;
			// Print list of nodes
			for(size_t i = offset; i < v.size(); ++i) {
				if(graph[v[i].node_id].distance == v[i].distance) {
					if(!v[i].processed) {
						++samples;
						sum += v[i].distance - base;
						if(samples < 50 || i >= v.size() - 50) {
							std::cout << v[i].distance/* - base*/;
							if(v[i].added + k >= v.size()) {
								std::cout << "*";
							}
							std::cout << ",";
						}
						else if(samples == 100) {
							std::cout << "..., ";
						}
					}
					else {
						++processed_samples;
						processed_sum += v[i].distance - base;
					}
				}
				else {
					pheet_assert(i != offset);
				}
			}
			if(samples > 0) {
				std::cout << std::endl << "Avg. Weight: " << (sum / (samples)) << " for " << samples << " nodes" << std::endl;
			}
			else {
				std::cout << std::endl;
			}
			if(processed_samples > 0) {
				std::cout << "Avg. Weight (incl processed > dist): " << ((sum + processed_sum) / (processed_samples + samples)) << " for " << (processed_samples + samples) << " nodes" << std::endl;
			}

			// Go through list and only process nodes visible to all threads and the first node (which is always processed)
			size_t orig_size = v.size();
			size_t sum_new = 0;
			size_t sum_upd = 0;
			size_t max_h = 0;
			size_t j, j2;
			for(j = 0, j2 = 0; j < block_size && offset + j2 < orig_size; ++j2) {
				n = v[offset + j2];
				size_t node = n.node_id;
				size_t d = graph[node].distance;
				size_t a = n.added;

				// Node is visible to all threads or the first node
				if(j == 0 || a + k < orig_size) {
					// Node has not been processed, and no better distance value has been found
					if(d == n.distance && !n.processed) {
						pc.num_actual_tasks.incr();

						size_t h = d - base;
						if(h > max_h) {
							max_h = h;
						}

						// relax node
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
								n.processed = false;
								v.push_back(n);
							}
						}
						v[offset + j2].processed = true;
						// Store in which phase node has been settled (will be overwritten if same node is settled again)
						settled[node] = phase;
						++j;
					}
				}
			}
			size_t max_h_rnd = max_h;
			// If not enough nodes visible to all threads are available process some random nodes
			if(orig_size - offset > 1 && j < block_size) {
				// Shuffle nodes for randomness
				std::random_shuffle(v.begin() + offset + 1, v.begin() + orig_size);

				for(j2 = 1;j < block_size && offset + j2 < orig_size; ++j2) {
					n = v[offset + j2];
					size_t node = n.node_id;
					size_t d = graph[node].distance;
					size_t a = n.added;

					// Node not visible to all threads
					if(a + k >= orig_size) {
						// Node has not been processed, and no better distance value has been found
						if(d == n.distance && !n.processed) {
							pc.num_actual_tasks.incr();

							size_t h = d - base;
							if(h > max_h_rnd) {
								max_h_rnd = h;
							}

							// relax node
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

			// Shuffle newly added nodes so the nodes not visible to all threads are really random
			std::random_shuffle(v.begin() + orig_size, v.end());
			for(size_t i2 = orig_size; i2 < v.size(); ++i2) {
				// Store information that tells us whether node is visible to all threads
				v[i2].added = i2;
			}

			++offset;
			std::cout << "New nodes found: " << sum_new << std::endl << "Better distance value found for " << sum_upd << " nodes" << std::endl;
			std::cout << "Maximum h: " << max_h << ", including randomly selected nodes: " << max_h_rnd << std::endl;
			std::cout << "------------------" << std::endl;
			// Sort nodes by distance value for next iteration
			std::sort(v.begin() + offset, v.end(), less);
		}

		for(size_t i = 0; i <= phase; ++i) {
			size_t counter = 0;
			for(size_t j = 0; j < size; ++j) {
				if(settled[j] == i) {
					++counter;
				}
			}
			std::cout << "Nodes settled in phase " << i << ": " << counter << std::endl;
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
