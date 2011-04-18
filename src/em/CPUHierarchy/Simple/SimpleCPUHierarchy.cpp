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

SimpleCPUHierarchy::SimpleCPUHierarchy(SimpleCPUHierarchy* parent, procs_t offset)
: num_levels(parent->num_levels - 1), offset(offset), levels(parent->levels + 1) {

}

SimpleCPUHierarchy::~SimpleCPUHierarchy() {
	if(level == 0) {
		delete []levels;
	}
	for(index_type i = 0; i < subsets.size(); i++) {
		delete subsets[i];
	}
	for(index_type i = 0; i < subsets.size(); i++) {
		delete cpus[i];
	}
}

procs_t get_size() {
	return levels[0];
}

vector<SimpleCPUHierarchy*> const* get_subsets() {
	if(num_levels > 1 && subsets.size() == 0) {
		assert((levels[0] % levels[1]) == 0);
		procs_t groups = levels[0] / levels[1];
		subsets.reserve(groups);

		for(procs_t i = 0; i < groups; i++) {
			SimpleCPUHierarchy* sub = new SimpleCPUHierarchy(this, offset + i*levels[1]);
			subsets.push_back(sub);
		}
	}
	return &subsets;
}

vector<CPUDescriptor*> const* get_cpus() {
	if(cpus.size() == 0) {
		cpus.reserve(get_size());

		for(procs_t i = 0; i < get_size(); i++) {
			CPUDescriptor desc = new CPUDescriptor(i + offset);
			cpus.push_back(desc);
		}
	}
	return &cpus;
}

}
