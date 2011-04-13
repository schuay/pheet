/*
 * SimpleCPUHierarchyCPUDescriptor.h
 *
 *  Created on: 13.04.2011
 *      Author: mwimmer
 */

#ifndef SIMPLECPUHIERARCHYCPUDESCRIPTOR_H_
#define SIMPLECPUHIERARCHYCPUDESCRIPTOR_H_

#include "../../../misc/types.h"

namespace pheet {

class SimpleCPUHierarchyCPUDescriptor {
public:
	SimpleCPUHierarchyCPUDescriptor(procs_t id);
	~SimpleCPUHierarchyCPUDescriptor();

	procs_t get_physical_id();
private:
	procs_t id;
};

}

#endif /* SIMPLECPUHIERARCHYCPUDESCRIPTOR_H_ */
