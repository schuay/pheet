/*
 * GraphBipartitioningTests.h
 *
 *  Created on: 07.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef GRAPHBIPARTITIONINGTESTS_H_
#define GRAPHBIPARTITIONINGTESTS_H_

#include "GraphBipartitioningTest.h"
#include "../../test_settings.h"

/*
 *
 */
namespace pheet {

class GraphBipartitioningTests {
public:
	GraphBipartitioningTests();
	~GraphBipartitioningTests();

	void run_test();

private:
	template<class Partitioner>
	void run_partitioner();
};

template <class Partitioner>
void GraphBipartitioningTests::run_partitioner() {
	for(size_t t = 0; t < sizeof(graph_bipartitioning_test_types)/sizeof(graph_bipartitioning_test_types[0]); t++) {
		for(size_t pr = 0; pr < sizeof(graph_bipartitioning_test_problems)/sizeof(graph_bipartitioning_test_problems[0]); pr++) {
/*			for(size_t n = 0; n < sizeof(graph_bipartitioning_test_n)/sizeof(graph_bipartitioning_test_n[0]); n++) {
				for(size_t p = 0; p < sizeof(graph_bipartitioning_test_p)/sizeof(graph_bipartitioning_test_p[0]); p++) {
					for(size_t max_w = 0; max_w < sizeof(graph_bipartitioning_test_max_w)/sizeof(graph_bipartitioning_test_max_w[0]); max_w++) {*/
			for(size_t c = 0; c < sizeof(graph_bipartitioning_test_cpus)/sizeof(graph_bipartitioning_test_cpus[0]); c++) {
				for(size_t s = 0; s < sizeof(graph_bipartitioning_test_seeds)/sizeof(graph_bipartitioning_test_seeds[0]); s++) {
					if(graph_bipartitioning_test_cpus[c] <= Partitioner::max_cpus) {
						GraphBipartitioningTest<Partitioner> gbt(graph_bipartitioning_test_cpus[c], graph_bipartitioning_test_types[t],
								graph_bipartitioning_test_problems[pr].n,
								graph_bipartitioning_test_problems[pr].p,
								graph_bipartitioning_test_problems[pr].max_w,
								graph_bipartitioning_test_seeds[s]);
						gbt.run_test();
					}
				}
			}
		}
	}
}

}

#endif /* GRAPHBIPARTITIONINGTESTS_H_ */
