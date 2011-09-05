/*
 * SimpleCPUHierarchy.cpp
 *
 *  Created on: 13.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SimpleCPUHierarchy.h"

#include <cstring>

namespace pheet {

SimpleCPUHierarchy::SimpleCPUHierarchy(procs_t np)
: level(0), num_levels(2), offset(0) {
	// Generates a flat hierarchy
	levels = new procs_t[2];
	levels[0] = np;
	levels[1] = 1;
}

SimpleCPUHierarchy::SimpleCPUHierarchy(procs_t* levels, procs_t num_levels)
: level(0), num_levels(num_levels), offset(0) {
	assert(num_levels >= 1);
	this->levels = new procs_t[num_levels];
	memcpy(this->levels, levels, sizeof(procs_t) * num_levels);
}

SimpleCPUHierarchy::SimpleCPUHierarchy(SimpleCPUHierarchy* parent, procs_t offset)
: level(parent->level + 1), num_levels(parent->num_levels - 1), offset(offset), levels(parent->levels + 1) {

}

SimpleCPUHierarchy::~SimpleCPUHierarchy() {
	if(level == 0) {
		delete []levels;
	}
	for(size_t i = 0; i < subsets.size(); i++) {
		delete subsets[i];
	}
	for(size_t i = 0; i < cpus.size(); i++) {
		delete cpus[i];
	}
}

procs_t SimpleCPUHierarchy::get_size() {
	return levels[0];
}

std::vector<SimpleCPUHierarchy*> const* SimpleCPUHierarchy::get_subsets() {
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

std::vector<SimpleCPUHierarchy::CPUDescriptor*> const* SimpleCPUHierarchy::get_cpus() {
	if(cpus.size() == 0) {
		cpus.reserve(get_size());

		for(procs_t i = 0; i < get_size(); i++) {
			SimpleCPUHierarchy::CPUDescriptor* desc = new SimpleCPUHierarchy::CPUDescriptor(i + offset);
			cpus.push_back(desc);
		}
	}
	return &cpus;
}

procs_t SimpleCPUHierarchy::get_max_depth() {
	return 1;
}

}
