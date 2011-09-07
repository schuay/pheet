/*
 * GraphBipartitioningTest.h
 *
 *  Created on: 07.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef GRAPHBIPARTITIONINGTEST_H_
#define GRAPHBIPARTITIONINGTEST_H_

#include "graph_helpers.h"
#include <vector>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>

/*
 *
 */
namespace pheet {

template <class Partitioner>
class GraphBipartitioningTest {
public:
	GraphBipartitioningTest(procs_t cpus, int type, size_t size, double p, unsigned int seed);
	~GraphBipartitioningTest();

	void run_test();

private:
	GraphVertex* generate_data();
	void delete_data(GraphVertex* data);

	procs_t cpus;
	int type;
	size_t size;
	float p;
	unsigned int seed;

	static char const* const types[];
};

template <class Partitioner>
char const* const GraphBipartitioningTest<Partitioner>::types[] = {"random"};

template <class Partitioner>
GraphBipartitioningTest<Partitioner>::GraphBipartitioningTest(procs_t cpus, int type, size_t size, float p, unsigned int seed)
: cpus(cpus), type(type), size(size), p(p), seed(seed) {

}

template <class Partitioner>
GraphBipartitioningTest<Partitioner>::~GraphBipartitioningTest() {

}

template <class Partitioner>
void GraphBipartitioningTest<Partitioner>::run_test() {
	GraphVertex* data = generate_data();

	Partitioner p(cpus, data, size);

	Time start, end;
	check_time(start);
	p.partition();
	check_time(end);

	size_t runs = get_number_of_runs(data);
	double seconds = calculate_seconds(start, end);
	std::cout << "test\tsorter\tscheduler\ttype\tsize\tp\tseed\tcpus\ttotal_time\truns\t";
	p.print_headers();
	std::cout << std::endl;
	cout << "graph_bipartitioning\t" << Partitioner::name << "\t" << Partitioner::scheduler_name << "\t" << types[type] << "\t" << size << "\t" << p << "\t" << seed << "\t" << cpus << "\t" << seconds << "\t" << runs << "\t";
	p.print_results();
	cout << endl;

	delete_data(data);
}

template <class Partitioner>
GraphVertex* GraphBipartitioningTest<Partitioner>::generate_data() {
	GraphVertex* data = new GraphVertex[size];

	boost::mt19937 rng;
    boost::uniform_real<float> rnd_f(0.0, 1.0);
    boost::uniform_int<size_t> rnd_st(0, 2048);

	vector<GraphEdge> edges;
	for(size_t i = 0; i < size - 1; ++i) {
		for(size_t j = i + 1; j < size; ++j) {
			if(rnd_f(rng) < p) {
				GraphEdge e;
				e.target = j;
				e.weight = rnd_st(rng);
				edges.add(e);
			}
		}
		data[i].num_edges = edges.size();
		if(edges.size() > 0) {
			data[i].edges = new GraphEdge[edges.size()];
			for(size_t j = 0; j < edges.size(); ++j) {
				data[i].edges[j] = edges[j];
			}
			edges.clear();
		}
		else {
			data[i].edges = NULL;
		}
	}
	data[size - 1].num_edges = 0;
	data[size - 1].edges = NULL;
}

template <class Partitioner>
void GraphBipartitioningTest<Partitioner>::delete_data(GraphVertex* data) {
	for(size_t i = 0; i < size - 1; ++i) {
		if(data[i].edges != NULL) {
			delete[] data[i].edges;
		}
	}
	delete[] data;
}

}

#endif /* GRAPHBIPARTITIONINGTEST_H_ */
