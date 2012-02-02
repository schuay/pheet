/*
 * HWLocSMTSMTMachineModel.h
 *
 *  Created on: Feb 1, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef HWLOCSMTMACHINEMODEL_H_
#define HWLOCSMTMACHINEMODEL_H_

#include "../../../settings.h"

#include <hwloc.h>


namespace pheet {

template <class Pheet>
class HWLocSMTTopologyInfo {
public:
	HWLocSMTTopologyInfo();
	~HWLocSMTTopologyInfo();

	hwloc_obj_t get_root_object();
	int get_total_depth();
	unsigned int get_total_width();

	hwloc_cpuset_t get_binding();
	void bind(hwloc_cpuset_t cpus);
private:
	HWLocSMTTopologyInfo(HWLocSMTTopologyInfo* topo, int depth);

	hwloc_topology_t topology;
	int total_depth;
};

template <class Pheet>
HWLocSMTTopologyInfo<Pheet>::HWLocSMTTopologyInfo() {
	/* Allocate and initialize topology object. */
	hwloc_topology_init(&topology);

	/* Perform the topology detection. */
	hwloc_topology_load(topology);

	total_depth = hwloc_get_type_or_below_depth(topology, HWLOC_OBJ_PU);
}

template <class Pheet>
HWLocSMTTopologyInfo<Pheet>::~HWLocSMTTopologyInfo() {
	hwloc_topology_destroy(topology);
}

template <class Pheet>
hwloc_obj_t HWLocSMTTopologyInfo<Pheet>::get_root_obj() {
	return hwloc_get_root_obj(topology);
}

template <class Pheet>
int HWLocSMTTopologyInfo<Pheet>::get_total_depth() {
	return total_depth;
}

template <class Pheet>
int HWLocSMTTopologyInfo<Pheet>::get_total_width() {
	return hwloc_get_nobjs_by_depth(topology, total_depth);
}

template <class Pheet>
hwloc_cpuset_t HWLocSMTTopologyInfo<Pheet>::get_binding() {
	hwloc_cpuset_t set;
	hwloc_get_cpubind(topology, set, HWLOC_CPUBIND_THREAD);
	return set;
}

template <class Pheet>
void HWLocSMTTopologyInfo<Pheet>::bind(hwloc_cpuset_t cpus) {
	hwloc_set_cpubind(topology, cpus, HWLOC_CPUBIND_THREAD);
}

template <class Pheet>
class HWLocSMTMachineModel {
public:
	typedef HWLocMachineModel<Pheet> ThisType;

	HWLocSMTMachineModel();
	HWLocSMTMachineModel(ThisType const& other);
	~HWLocSMTMachineModel();

	ThisType& operator=(ThisType const& other);

	bool is_leaf();
	procs_t get_num_children();
	HWLocSMTMachineModel<Pheet> get_child(procs_t id);
	procs_t get_node_id();
	procs_t get_node_offset();
	procs_t get_num_leaves();

	void bind();
	void unbind();

private:
	HWLocSMTMachineModel(HWLocSMTTopologyInfo* topo, hwloc_obj_t node, int depth);
	HWLocSMTTopologyInfo* topo;
	hwloc_obj_t node;
	int depth;
	bool root;

	hwloc_cpuset_t prev_binding;
#ifndef NDEBUG
	bool bound;
#endif
};

template <class Pheet>
HWLocSMTMachineModel<Pheet>::HWLocSMTMachineModel()
: topo(new HWLocSMTTopologyInfo()), depth(0), root(true) {
#ifndef NDEBUG
	bound = false;
#endif
}

template <class Pheet>
HWLocSMTMachineModel<Pheet>::HWLocSMTMachineModel(HWLocSMTMachineModel<Pheet> const& other)
: topo(other.topo), depth(other.depth), root(false) {
#ifndef NDEBUG
	bound = false;
#endif
}

template <class Pheet>
HWLocSMTMachineModel<Pheet>::HWLocSMTMachineModel(HWLocSMTTopologyInfo* topo, hwloc_obj_t node)
: topo(topo), node(node), root(false) {

}

template <class Pheet>
HWLocSMTMachineModel<Pheet>::~HWLocSMTMachineModel() {
	if(root) {
		delete topo;
	}
}

template <class Pheet>
procs_t HWLocSMTMachineModel<Pheet>::get_num_children() {
	return node->arity;
}

template <class Pheet>
HWLocSMTMachineModel<Pheet> HWLocSMTMachineModel<Pheet>::get_child(procs_t id) {
	assert(id < node.arity);
	assert(depth < (topo->get_total_depth() - 1));
	return HWLocSMTMachineModel(topo, node.children[id], depth + 1);
}

template <class Pheet>
HWLocSMTMachineModel<Pheet> HWLocSMTMachineModel<Pheet>::is_leaf() {
	return depth >= (topo->get_total_depth() - 1);
}

template <class Pheet>
void HWLocSMTMachineModel<Pheet>::bind() {
#ifndef NDEBUG
	assert(!bound);
	bound = true;
#endif
	prev_binding = topology->get_binding();
	topology->bind(node->cpuset);
}

template <class Pheet>
void HWLocSMTMachineModel<Pheet>::unbind() {
#ifndef NDEBUG
	assert(bound);
	bound = false;
#endif
	topology->bind(prev_binding);
}

template <class Pheet>
procs_t HWLocSMTMachineModel<Pheet>::get_node_id() {
	return node->logical_index;
}

template <class Pheet>
procs_t HWLocSMTMachineModel<Pheet>::get_node_offset() {
	hwloc_obj_t tmp = node;
	while(tmp->depth < (topo->get_total_depth() - 1)) {
		assert(tmp->arity != 0);
		tmp = tmp->children[0];
	}
	return tmp->logical_index;
}

template <class Pheet>
procs_t HWLocSMTMachineModel<Pheet>::get_num_leaves() {
	if(depth == (topo->get_total_depth() - 1)) {
		return 1;
	}
	else if(depth == 0) {
		return topology->get_total_width();
	}
	else {
		// Expensive, so try not using it
		procs_t ret = 0;
		for(procs_t i = 0; i < get_num_children(); ++i) {
			ret += get_child(i).get_num_leaves();
		}
	}
	return node->logical_index;
}

}


#endif /* HWLOCSMTMACHINEMODEL_H_ */
