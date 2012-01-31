/*
 * HWLocMachineModel.h
 *
 *  Created on: Jan 31, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef HWLOCMACHINEMODEL_H_
#define HWLOCMACHINEMODEL_H_

#include "../../../settings.h"

#include <hwloc.h>


namespace pheet {

template <class Pheet>
class HWLocTopologyInfo {
public:
	HWLocTopologyInfo();
	~HWLocTopologyInfo();

	hwloc_obj_t get_root_object();
	int get_total_depth();

	hwloc_cpuset_t get_binding();
	void bind(hwloc_cpuset_t cpus);
private:
	HWLocTopologyInfo(HWLocTopologyInfo* topo, int depth);

	hwloc_topology_t topology;
	int total_depth;
};

template <class Pheet>
HWLocTopologyInfo<Pheet>::HWLocTopologyInfo() {
	/* Allocate and initialize topology object. */
	hwloc_topology_init(&topology);

	/* Perform the topology detection. */
	hwloc_topology_load(topology);

	total_depth = hwloc_get_type_or_below_depth(topology, HWLOC_OBJ_CORE);
}

template <class Pheet>
HWLocTopologyInfo<Pheet>::~HWLocTopologyInfo() {
	hwloc_topology_destroy(topology);
}

template <class Pheet>
hwloc_obj_t HWLocTopologyInfo<Pheet>::get_root_obj() {
	return hwloc_get_root_obj(topology);
}

template <class Pheet>
int HWLocTopologyInfo<Pheet>::get_total_depth() {
	return total_depth;
}

template <class Pheet>
hwloc_cpuset_t HWLocTopologyInfo<Pheet>::get_binding() {
	hwloc_cpuset_t set;
	hwloc_get_cpubind(topology, set, HWLOC_CPUBIND_THREAD);
	return set;
}

template <class Pheet>
void HWLocTopologyInfo<Pheet>::bind(hwloc_cpuset_t cpus) {
	hwloc_set_cpubind(topology, cpus, HWLOC_CPUBIND_THREAD);
}

template <class Pheet>
class HWLocMachineModel {
public:
	HWLocMachineModel();
	~HWLocMachineModel();

	bool is_leaf();
	procs_t get_num_children();
	HWLocMachineModel<Pheet> get_child(procs_t id);
	procs_t get_node_id();

	void bind();
	void unbind();

private:
	HWLocMachineModel(HWLocTopologyInfo* topo, hwloc_obj_t node, int depth);
	HWLocTopologyInfo* topo;
	hwloc_obj_t node;
	int depth;

	hwloc_cpuset_t prev_binding;
#ifndef NDEBUG
	bool bound;
#endif
};

template <class Pheet>
HWLocMachineModel<Pheet>::HWLocMachineModel()
: topo(new HWLocTopologyInfo()), depth(0) {
#ifndef NDEBUG
	bound = false;
#endif
}

template <class Pheet>
HWLocMachineModel<Pheet>::HWLocMachineModel(HWLocTopologyInfo* topo, hwloc_obj_t node, int depth)
: topo(topo), node(node), depth(depth) {

}

template <class Pheet>
HWLocMachineModel<Pheet>::~HWLocMachineModel() {
	if(depth == 0) {
		delete topo;
	}
}

template <class Pheet>
procs_t HWLocMachineModel<Pheet>::get_num_children() {
	return node->arity;
}

template <class Pheet>
HWLocMachineModel<Pheet> HWLocMachineModel<Pheet>::get_child(procs_t id) {
	assert(id < node->arity);
	assert(depth < (topo->get_total_depth() - 1));
	return HWLocMachineModel(topo, node->children[id], depth + 1);
}

template <class Pheet>
HWLocMachineModel<Pheet> HWLocMachineModel<Pheet>::is_leaf() {
	return depth >= (topo->get_total_depth() - 1);
}

template <class Pheet>
void HWLocMachineModel<Pheet>::bind() {
#ifndef NDEBUG
	assert(!bound);
	bound = true;
#endif
	prev_binding = topology->get_binding();
	topology->bind(node->cpuset);
}

template <class Pheet>
void HWLocMachineModel<Pheet>::unbind() {
#ifndef NDEBUG
	assert(bound);
	bound = false;
#endif
	topology->bind(prev_binding);
}

template <class Pheet>
procs_t HWLocMachineModel<Pheet>::get_node_id() {
	return node->logical_index;
}

}

#endif /* HWLOCMACHINEMODEL_H_ */
