/*
 * PrefixSumTest.h
 *
 *  Created on: Jun 28, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef PREFIXSUMTEST_H_
#define PREFIXSUMTEST_H_

#include <stdlib.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <pheet/pheet.h>
#include "../Test.h"

//using namespace std;

namespace pheet {

template <typename T, unsigned alignment>
class aligned_data
{
	void* raw;
	void* aligned;
public:
	aligned_data(): raw(), aligned() {}

	aligned_data(size_t elements)
	{
		size_t size = sizeof(T) * elements;
		size_t totalSize = size + alignment;
		raw = malloc(totalSize);
		aligned = raw;
		aligned = std::align(alignment, size, aligned, totalSize);
		if (aligned == 0)
			throw std::runtime_error("Alignment of allocation failed");
	}

	aligned_data(aligned_data && other) :
		raw(std::move(other.raw)),
		aligned(std::move(other.aligned))
	{
		other.raw = nullptr;
		other.aligned = nullptr;
	}

	aligned_data& operator=(aligned_data&& other) {
		raw = std::move(other.raw);
		aligned = std::move(other.aligned);
		other.raw = nullptr;
		other.aligned = nullptr;
		return *this;
	}

	~aligned_data() { free(raw); }

	T* ptr() { return static_cast<T*>(aligned); }
};

template <class Pheet, template <class P> class Algorithm>
class PrefixSumTest : Test {
public:
	PrefixSumTest(procs_t cpus, size_t num_problems, int type, size_t size, unsigned int seed);
	virtual ~PrefixSumTest();

	void run_test();

private:
	typedef aligned_data<unsigned int, 64> TestData;

	TestData generate_data();
	bool is_correct(unsigned int* data);

	procs_t cpus;
	size_t num_problems;
	int type;
	size_t size;
	unsigned int seed;

	static char const* const types[];
};

template <class Pheet, template <class P> class Algorithm>
char const* const PrefixSumTest<Pheet, Algorithm>::types[] = {"random", "gauss", "equal", "bucket", "staggered"};

template <class Pheet, template <class P> class Algorithm>
PrefixSumTest<Pheet, Algorithm>::PrefixSumTest(procs_t cpus, size_t num_problems, int type, size_t size, unsigned int seed)
: cpus(cpus), num_problems(num_problems), type(type), size(size), seed(seed) {

}

template <class Pheet, template <class P> class Algorithm>
PrefixSumTest<Pheet, Algorithm>::~PrefixSumTest() {

}

template <class Pheet, template <class P> class Algorithm>
void PrefixSumTest<Pheet, Algorithm>::run_test() {
	TestData* data = new TestData[num_problems];
	for(size_t i = 0; i < num_problems; ++i) {
		data[i] = generate_data();
	}

	typename Pheet::Environment::PerformanceCounters pc;
	typename Algorithm<Pheet>::PerformanceCounters apc;

	Time start, end;

	{typename Pheet::Environment env(cpus, pc);
		check_time(start);
		{typename Pheet::Finish f;
			for(size_t i = 0; i < num_problems; ++i) {
				Pheet::template
					spawn<Algorithm<Pheet> >(data[i].ptr(), size, apc);
			}
		}

		check_time(end);
	}

	bool correctness = true;
	for(size_t i = 0; i < num_problems; ++i) {
		correctness &= is_correct(data[i].ptr());
	}
	double seconds = calculate_seconds(start, end);
	std::cout << "test\talgorithm\tscheduler\tnum_problems\ttype\tsize\tseed\tcpus\ttotal_time\tcorrect\t";
	Pheet::Environment::PerformanceCounters::print_headers();
	Algorithm<Pheet>::PerformanceCounters::print_headers();
	std::cout << std::endl;
	std::cout << "prefix_sum\t" << Algorithm<Pheet>::name << "\t";
	Pheet::Environment::print_name();
	std::cout << "\t" << num_problems << "\t" << types[type] << "\t" << size << "\t" << seed << "\t" << cpus << "\t" << seconds << "\t" << correctness << "\t";
	pc.print_values();
	apc.print_values();
	std::cout << std::endl;
	
	delete[] data;
}


template <class Pheet, template <class P> class Algorithm>
typename PrefixSumTest<Pheet, Algorithm>::TestData PrefixSumTest<Pheet, Algorithm>::generate_data() {

	TestData testData(size);
	unsigned int* data = testData.ptr();
	
//	std::mt19937 rng;
//	rng.seed(seed);

	switch(type) {
	case 0:
		// 1s
	{
		for(size_t i = 0; i < size; i++) {
			data[i] = 1;
		}
	}
		break;
	break;
	default:
		throw "unknown type for prefix sum";
	}

	return testData;
}

template <class Pheet, template <class P> class Algorithm>
bool PrefixSumTest<Pheet, Algorithm>::is_correct(unsigned int* data) {
	for(size_t i = 0; i < size; i++) {
		if(data[i] != i + 1) {
			std::cout << ">>" << i << ": " << data[i] << std::endl;
			return false;
		}
	}
	return true;
}

} /* namespace pheet */
#endif /* PREFIXSUMTEST_H_ */
