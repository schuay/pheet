/*
 * SsspTest.h
 *
 *  Created on: Jul 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef SSSPTEST_H_
#define SSSPTEST_H_

#include <pheet/pheet.h>
#include "Test.h"
#include "util/Graph/Graph.h"

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <random>
#include <limits>

namespace pheet {

template <class Pheet, template <class P> class Algorithm>
class SsspTest : Test {
public:
	SsspTest(procs_t cpus, int type, size_t k, size_t size, float p, size_t max_w, unsigned int seed);
	~SsspTest();

	void run_test();

private:
	SsspGraphVertex* generate_data();
	void delete_data(SsspGraphVertex* data);
	bool check_solution(SsspGraphVertex* data);
	static void check_vertex(SsspGraphVertex* data, size_t i, bool& correct);

	procs_t cpus;
	int type;
	size_t k;
	size_t size;
	float p;
	size_t max_w;
	unsigned int seed;

	static char const* const types[];
};

template <class Pheet, template <class P> class Algorithm>
char const* const SsspTest<Pheet, Algorithm>::types[] = {"random"};

template <class Pheet, template <class P> class Algorithm>
SsspTest<Pheet, Algorithm>::SsspTest(procs_t cpus, int type, size_t k, size_t size, float p, size_t max_w, unsigned int seed)
: cpus(cpus), type(type), k(k), size(size), p(p), max_w(max_w), seed(seed) {

}

template <class Pheet, template <class P> class Algorithm>
SsspTest<Pheet, Algorithm>::~SsspTest() {

}

template <class Pheet, template <class P> class Algorithm>
void SsspTest<Pheet, Algorithm>::run_test() {
	SsspGraph* graph = graph_random(size, p, max_w, seed);
	SsspGraphVertex* data = graph->vertices;

	typename Pheet::Environment::PerformanceCounters pc;
	typename Algorithm<Pheet>::PerformanceCounters ppc;

	Time start, end;
	{typename Pheet::Environment env(cpus, pc);
		Algorithm<Pheet>::set_k(k);
		check_time(start);
		Pheet::template
			finish<Algorithm<Pheet> >(data, size, ppc);
		check_time(end);
	}

	bool correct = check_solution(data);
	double seconds = calculate_seconds(start, end);
	std::cout << "test\talgorithm\tscheduler\ttype\tsize\tp\tmax_w\tk\tseed\tcpus\ttotal_time\tcorrect\t";
//	Algorithm<Pheet>::print_headers();
	Algorithm<Pheet>::PerformanceCounters::print_headers();
	Pheet::Environment::PerformanceCounters::print_headers();
	std::cout << std::endl;
	std::cout << "sssp\t" << Algorithm<Pheet>::name << "\t";
	Pheet::Environment::print_name();
	std::cout << "\t" << types[type] << "\t" << size << "\t" << p << "\t" << max_w << "\t" << k << "\t" << seed << "\t" << cpus << "\t" << seconds << "\t" << correct << "\t";
//	Algorithm<Pheet>::print_configuration();
	ppc.print_values();
	pc.print_values();
	std::cout << std::endl;

	graph_free(graph);
}

template <class Pheet, template <class P> class Algorithm>
bool SsspTest<Pheet, Algorithm>::check_solution(SsspGraphVertex* data) {
	bool correct = true;
	{pheet::Pheet::Environment p;
		for(size_t i = 0; i < size; ++i) {
			pheet::Pheet::spawn(SsspTest<Pheet, Algorithm>::check_vertex, data, i, correct);
		}
	}

	return correct;
}

template <class Pheet, template <class P> class Algorithm>
void SsspTest<Pheet, Algorithm>::check_vertex(SsspGraphVertex* data, size_t i, bool& correct) {
	for(size_t j = 0; j < data[i].num_edges; ++j) {
		if(data[data[i].edges[j].target].distance > data[i].distance + data[i].edges[j].weight) {
			correct = false;
			return;
		}
	}
}

} /* namespace pheet */
#endif /* SSSPTEST_H_ */
