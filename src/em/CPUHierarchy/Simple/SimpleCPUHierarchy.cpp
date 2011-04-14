/*
 * SimpleCPUHierarchy.cpp
 *
 *  Created on: 13.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SimpleCPUHierarchy.h"

namespace pheet {

SimpleCPUHierarchy::SimpleCPUHierarchy(procs_t np)
: num_levels(2), offset(0) {
	// Generates a flat hierarchy
	levels = new procs_t[2];
	levels[0] = np;
	levels[1] = 1;
}

SimpleCPUHierarchy::SimpleCPUHierarchy(procs_t* levels, procs_t num_levels)
: num_levels(num_levels), offset(0) {
	assert(num_levels >= 1);
	this->levels = new procs_t[num_levels];
	memcpy(this->levels, levels, sizeof(procs_t) * num_levels);
}

SimpleCPUHierarchy::SimpleCPUHierarchy(SimpleCPUHierarchy& parent, procs_t offset)
: num_levels(root.num_levels - 1), offset(offset), levels(parent.levels + 1) {

}

SimpleCPUHierarchy::~SimpleCPUHierarchy() {
	if(level == 0) {
		delete []levels;
	}
}

procs_t get_size() {
	return levels[0];
}

vector<SimpleCPUHierarchy> get_subsets() {
	vector<SimpleCPUHierarchy> ret;
	if(num_levels == 1) {
		return ret;
	}

	assert((levels[0] % levels[1]) == 0);
	procs_t groups = levels[0] / levels[1];
	ret.reserve(groups);

	for(procs_t i = 0; i < groups; i++) {
		SimpleCPUHierarchy sub(*this, offset + i*levels[1]);
		ret.push_back(sub);
	}
	return ret;
}

vector<CPUDescriptor> get_cpus() {
	vector<CPUDescriptor> ret;
	ret.reserve(get_size());

	for(procs_t i = 0; i < get_size(); i++) {
		CPUDescriptor desc(i + offset);
		ret.push_back(desc);
	}
	return ret;
}

}
