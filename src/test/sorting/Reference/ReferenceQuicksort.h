/*
 * ReferenceQuicksort.h
 *
 *  Created on: 21.04.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef REFERENCEQUICKSORT_H_
#define REFERENCEQUICKSORT_H_

/*
 *
 */
namespace pheet {

class ReferenceQuicksort {
public:
	ReferenceQuicksort(procs_t cpus, unsigned int* data, size_t length);
	~ReferenceQuicksort();

	void sort();

	static const procs_t max_cpus;
	static const char name[];
	static const char scheduler_name[];

private:
	void sort(unsigned int* data, size_t length);

	unsigned int* data;
	size_t length;
};

}

#endif /* REFERENCEQUICKSORT_H_ */
