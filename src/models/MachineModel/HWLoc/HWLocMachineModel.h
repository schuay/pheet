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

	hwloc_obj_t get_root_obj();
	int get_total_depth();
	unsigned int get_total_width();

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
unsigned int HWLocTopologyInfo<Pheet>::get_total_width() {
	return hwloc_get_nbobjs_by_depth(topology, total_depth);
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
	typedef HWLocMachineModel<Pheet> ThisType;

	HWLocMachineModel();
	HWLocMachineModel(HWLocMachineModel<Pheet> const& other);
	~HWLocMachineModel();

	ThisType& operator=(ThisType const& other);

	bool is_leaf();
	procs_t get_num_children();
	HWLocMachineModel<Pheet> get_child(procs_t id);
	procs_t get_node_id();
	procs_t get_num_leaves();
	procs_t get_memory_level();

	void bind();
	void unbind();

private:
	HWLocMachineModel(HWLocTopologyInfo<Pheet>* topo, hwloc_obj_t node);
	HWLocTopologyInfo<Pheet>* topo;
	hwloc_obj_t node;
	bool root;

	hwloc_cpuset_t prev_binding;
#ifndef NDEBUG
	bool bound;
#endif
};

template <class Pheet>
HWLocMachineModel<Pheet>::HWLocMachineModel()
: topo(new HWLocTopologyInfo<Pheet>()), node(topo->get_root_obj()), root(true) {
#ifndef NDEBUG
	bound = false;
#endif
}

template <class Pheet>
HWLocMachineModel<Pheet>::HWLocMachineModel(HWLocMachineModel<Pheet> const& other)
: topo(other.topo), node(other.node), root(false) {
#ifndef NDEBUG
	bound = false;
#endif
}

template <class Pheet>
HWLocMachineModel<Pheet>::HWLocMachineModel(HWLocTopologyInfo<Pheet>* topo, hwloc_obj_t node)
: topo(topo), node(node), root(false) {
#ifndef NDEBUG
	bound = false;
#endif
}

template <class Pheet>
HWLocMachineModel<Pheet>::~HWLocMachineModel() {
	if(root) {
		delete topo;
	}
}

template <class Pheet>
HWLocMachineModel<Pheet>& HWLocMachineModel<Pheet>::operator=(HWLocMachineModel<Pheet> const& other) {
	assert(topo == other.topo || !root);
	topo = other.topo;
}

template <class Pheet>
procs_t HWLocMachineModel<Pheet>::get_num_children() {
	return node->arity;
}

template <class Pheet>
HWLocMachineModel<Pheet> HWLocMachineModel<Pheet>::get_child(procs_t id) {
	assert(id < node->arity);
	assert(node->depth < (topo->get_total_depth() - 1));
	return HWLocMachineModel(topo, node->children[id]);
}

template <class Pheet>
bool HWLocMachineModel<Pheet>::is_leaf() {
	return node->depth >= (topo->get_total_depth() - 1);
}

template <class Pheet>
void HWLocMachineModel<Pheet>::bind() {
#ifndef NDEBUG
	assert(!bound);
	bound = true;
#endif
	prev_binding = topo->get_binding();
	topo->bind(node->cpuset);
}

template <class Pheet>
void HWLocMachineModel<Pheet>::unbind() {
#ifndef NDEBUG
	assert(bound);
	bound = false;
#endif
	topo->bind(prev_binding);
}

template <class Pheet>
procs_t HWLocMachineModel<Pheet>::get_node_id() {
	return node->logical_index;
}

template <class Pheet>
procs_t HWLocMachineModel<Pheet>::get_num_leaves() {
	if(node->depth == (topo->get_total_depth() - 1)) {
		return 1;
	}
	else if(node->depth == 0) {
		return topo->get_total_width();
	}
	else {
		// TODO: can be improved by just checking the node offset of the rightmost child
		// Expensive, so try not using it
		procs_t ret = 0;
		for(procs_t i = 0; i < get_num_children(); ++i) {
			ret += get_child(i).get_num_leaves();
		}
	}
	return node->logical_index;
}


template <class Pheet>
procs_t HWLocMachineModel<Pheet>::get_memory_level() {
	return node->depth;
}

}

#endif /* HWLOCMACHINEMODEL_H_ */
