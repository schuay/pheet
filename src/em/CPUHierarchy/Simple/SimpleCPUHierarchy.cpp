/*
 * SimpleCPUHierarchy.cpp
 *
 *  Created on: 13.04.2011
 *      Author: mwimmer
 */

#include "SimpleCPUHierarchy.h"

namespace pheet {

SimpleCPUHierarchy::SimpleCPUHierarchy(procs_t np)
: num_levels(2), level(0) offset(0) {
	// Generates a flat hierarchy
	levels = new procs_t[2];
	levels[0] = np;
	levels[1] = 1;
}

SimpleCPUHierarchy::SimpleCPUHierarchy(procs_t np, procs_t *levels, procs_t num_levels)
: num_levels(num_levels), level(0), offset(0) {
	this->levels = new procs_t[num_levels];
	memcpy(this->levels, levels, sizeof(procs_t) * num_levels);
}

SimpleCPUHierarchy::SimpleCPUHierarchy(SimpleCPUHierarchy *root, procs_t level, procs_t offset)
: num_levels(root->num_levels - level), level(level), offset(offset), levels(root->levels + level) {

}

SimpleCPUHierarchy::~SimpleCPUHierarchy() {
	if(level == 0) {
		delete []levels;
	}
}

}
