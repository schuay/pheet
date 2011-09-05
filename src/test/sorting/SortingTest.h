/*
 * SortingTestBase.h
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SORTINGTEST_H_
#define SORTINGTEST_H_

#include <stdlib.h>
#include <iostream>
#include "../../misc/types.h"
#include "../Test.h"
#include <exception>

using namespace std;

namespace pheet {

template <class Sorter>
class SortingTest : Test {
public:
	SortingTest(procs_t cpus, int type, size_t size, unsigned int seed);
	virtual ~SortingTest();

	void run_test();

private:
	unsigned int* generate_data();
	size_t get_number_of_runs(unsigned int* data);

	procs_t cpus;
	int type;
	size_t size;
	unsigned int seed;

	static char const* const types[];
};

template <class Sorter>
char const* const SortingTest<Sorter>::types[] = {"random", "gauss", "equal", "bucket", "staggered"};

template <class Sorter>
SortingTest<Sorter>::SortingTest(procs_t cpus, int type, size_t size, unsigned int seed)
: cpus(cpus), type(type), size(size), seed(seed) {

}

template <class Sorter>
SortingTest<Sorter>::~SortingTest() {

}

template <class Sorter>
void SortingTest<Sorter>::run_test() {
	unsigned int* data = generate_data();

	Sorter s(cpus, data, size);

	Time start, end;
	check_time(start);
	s.sort();
	check_time(end);

	size_t runs = get_number_of_runs(data);
	double seconds = calculate_seconds(start, end);
	std::cout << "test\tsorter\tscheduler\ttype\tsize\tseed\tcpus\ttotal_time\truns\t";
	s.print_headers();
	std::cout << std::endl;
	cout << "sorting\t" << Sorter::name << "\t" << Sorter::scheduler_name << "\t" << types[type] << "\t" << size << "\t" << seed << "\t" << cpus << "\t" << seconds << "\t" << runs << "\t";
	s.print_results();
	cout << endl;
	delete[] data;
}


template <class Sorter>
unsigned int* SortingTest<Sorter>::generate_data() {
	unsigned int* data = new unsigned int[size];

	srandom(seed);
	switch(type) {
	case 0:
		// Random
		for(size_t i = 0; i < size; i++) {
			data[i] = random() % 0x3FFFFFF;
		}
		break;
	case 1:
	{
		// Gauss ?
		int temp;
		for(size_t i = 0; i < size; i++) {
			temp=0;
			for (int j=0;j<4;j++)
				temp += random() % 0x0FFFFFF;
			temp /=4;
			data[i] = temp;
		}
	}
		break;
	case 2:
	{
		// All the same
		int temp = random() % 0x3FFFFFF;
		for(size_t i = 0; i < size; i++) {
			data[i] = temp;
		}
	}
		break;
	case 3:
	{
		// Bucket
		size_t temp = 0x3FFFFFF / cpus + 1;
		size_t c=0;
		for (procs_t i=0;i<cpus;i++)
		{
			procs_t j;
			size_t k;
		  for (j=0;j<cpus;j++)
		  {
			 for(k=0;k<size/cpus/cpus;k++)
			 {
			 int t2 = j * temp;
			 data[c] = t2 + ((random() & 0x3FFFFFF) / cpus);
			 c++;
			 }
		  }
		}

		for(;c<size;c++)
		{
			data[c]=random() & 0x3FFFFFF;
		}
	}
		break;
	case 4:
	{
		// Staggered
		size_t c=0, temp;
		for(procs_t i=0;i<cpus;i++)
		{
			size_t j;
			if (i < (cpus/2))
				temp = 2*cpus + 1;
			else
					temp = (i - (cpus/2))*2;

			temp = temp*((0x3FFFFFF / cpus) + 1);

			for (j=0;j<size/cpus;j++)
			{
			  data[c] = temp + ((random() & 0x3FFFFFF)/cpus);
			  c++;
			}
		}
		for(;c<size;c++)
		{
			data[c]=random() & 0x3FFFFFF;
		}
	}
	break;
	default:
		throw "unknown type for sorter";
	}

	return data;
}

template <class Sorter>
size_t SortingTest<Sorter>::get_number_of_runs(unsigned int* data) {
	unsigned int prev = data[0];
	size_t runs = 1;
//	cout << data[0];
	for(size_t i = 1; i < size; i++) {
		unsigned int current = data[i];
//		cout << "\t" << current;
		if(current < prev) {
		//	cout << "new run starts at " << i << " -2: " << data[i-2] << " -1: " << data[i-1] << " 0: " << data[i] << " 1: " << data[i+1] << " 2: " << data[i+2] << endl;
			runs++;
		}
		prev = current;
	}
	return runs;
}

}

#endif /* SORTINGTEST_H_ */
