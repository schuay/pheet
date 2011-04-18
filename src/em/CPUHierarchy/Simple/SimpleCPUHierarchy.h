/*
 * SimpleCPUHierarchy.h
 *
 *  Created on: 13.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SIMPLECPUHIERARCHY_H_
#define SIMPLECPUHIERARCHY_H_

#include "../../../misc/types.h"

#include "SimpleCPUHierarchyCPUDescriptor.h"

#include <vector>

namespace pheet {


class SimpleCPUHierarchy {
public:
	typedef SimpleCPUHierarchyCPUDescriptor CPUDescriptor;

	SimpleCPUHierarchy(procs_t np);
	SimpleCPUHierarchy(procs_t np, procs_t* levels, procs_t num_levels);
	~SimpleCPUHierarchy();

	procs_t get_size();
	vector<SimpleCPUHierarchy*> const* get_subsets();
	vector<CPUDescriptor*> const* get_cpus();

private:
	SimpleCPUHierarchy(SimpleCPUHierarchy* parent, procs_t offset);

	procs_t num_levels;
	procs_t offset;
	procs_t* levels;
	vector<SimpleCPUHierarchy*> subsets;
	vector<CPUDescriptor*> cpus;
};

}

#endif /* SIMPLECPUHIERARCHY_H_ */
