/*
 * BinaryTreeMachineModel.h
 *
 *  Created on: Feb 1, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef BINARYTREEMACHINEMODEL_H_
#define BINARYTREEMACHINEMODEL_H_

namespace pheet {

template <class Pheet, class BaseModelT>
class BinaryTreeMachineModel {
public:
	typedef BinaryTreeMachineModel<Pheet, BaseModelT> Self;
	typedef BaseModelT BaseModel;

	BinaryTreeMachineModel();
	BinaryTreeMachineModel(Self& other);
	~BinaryTreeMachineModel();

	bool is_leaf();
	procs_t get_num_children();
	Self get_child(procs_t id);
	procs_t get_node_id();
	procs_t get_node_offset();
	procs_t get_num_leaves();
	procs_t get_memory_level();

	void bind();
	void unbind();

private:
	BinaryTreeMachineModel(BaseModel& base, procs_t first_child, procs_t last_child, procs_t node_id);
	BaseModel base;
	procs_t first_child;
	procs_t last_child;
	procs_t node_id;
};

template <class Pheet, class BaseModelT>
BinaryTreeMachineModel<Pheet, BaseModelT>::BinaryTreeMachineModel()
: first_child(0), last_child(0), node_id(0) {
	assert(base.is_leaf() || base.get_num_children() > 0);
	while((!base.is_leaf()) && base.get_num_children() == 1) {
		base = base.get_child(0);
	}
	if(!base.is_leaf()) {
		last_child = base.get_num_children() - 1;
		assert(last_child > first_child);
	}
}

template <class Pheet, class BaseModelT>
BinaryTreeMachineModel<Pheet, BaseModelT>::BinaryTreeMachineModel(Self& other)
: base(other.base), first_child(other.first_child), last_child(other.last_child), node_id(other.node_id) {
	assert(base.is_leaf() || base.get_num_children() > 1);
	assert(base.is_leaf() || last_child > first_child);
}

template <class Pheet, class BaseModelT>
BinaryTreeMachineModel<Pheet, BaseModelT>::BinaryTreeMachineModel(BaseModel& base, procs_t first_child, procs_t last_child, procs_t node_id)
: base(base), first_child(first_child), last_child(last_child), node_id(node_id) {
	assert(base.is_leaf() || last_child >= first_child);
	if(!base.is_leaf() && (last_child == first_child)) {
		do{
			base = base.get_child(0);
		} while((!base.is_leaf()) && base.get_num_children() == 1);
		first_child = 0;
		if(base.is_leaf()) {
			last_child = 0;
		}
		else {
			last_child = base.get_num_children() - 1;
			assert(last_child > first_child);
		}
	}
}

template <class Pheet, class BaseModelT>
BinaryTreeMachineModel<Pheet, BaseModelT>::~BinaryTreeMachineModel() {

}

template <class Pheet, class BaseModelT>
bool BinaryTreeMachineModel<Pheet, BaseModelT>::is_leaf() {
	return base.is_leaf();
}

template <class Pheet, class BaseModelT>
procs_t BinaryTreeMachineModel<Pheet, BaseModelT>::get_num_children() {
	assert(!is_leaf());
	return 2;
}

template <class Pheet, class BaseModelT>
BinaryTreeMachineModel<Pheet, BaseModelT> BinaryTreeMachineModel<Pheet, BaseModelT>::get_child(procs_t id) {
	assert(!is_leaf());
	assert(last_child > first_child);
	procs_t middle = first_child + ((last_child - first_child) >> 1);
	if(id == 0) {
		return Self(base, first_child, middle, 0);
	}
	else {
		assert(id == 1);
		return Self(base, middle + 1, last_child, 1);
	}
}

template <class Pheet, class BaseModelT>
procs_t BinaryTreeMachineModel<Pheet, BaseModelT>::get_node_id() {
	return node_id;
}

template <class Pheet, class BaseModelT>
procs_t BinaryTreeMachineModel<Pheet, BaseModelT>::get_node_offset() {
	if(first_child == 0) {
		return base.get_node_id();
	}
	else {
		assert(!is_leaf());
		return base.get_child(first_child).get_node_id();
	}
}

template <class Pheet, class BaseModelT>
procs_t BinaryTreeMachineModel<Pheet, BaseModelT>::get_num_leaves() {
	if(base.is_leaf()) {
		return base.get_num_leaves();
	}
	else if(first_child == 0 && last_child == (base.get_num_children() - 1)) {
		return base.get_num_leaves();
	}
	else {
		assert(last_child > first_child);
		procs_t ret = 0;
		for(procs_t i = first_child; i <= last_child; ++i) {
			ret += base.get_child(i).get_num_leaves();
		}
		return ret;
	}
}

template <class Pheet, class BaseModelT>
void BinaryTreeMachineModel<Pheet, BaseModelT>::bind() {
	assert(base.is_leaf() || (first_child == 0 && last_child == (base.get_num_children() - 1)));
	base.bind();
}

template <class Pheet, class BaseModelT>
void BinaryTreeMachineModel<Pheet, BaseModelT>::unbind() {
	assert(base.is_leaf() || (first_child == 0 && last_child == (base.get_num_children() - 1)));
	base.unbind();
}

template <class Pheet, class BaseModelT>
procs_t BinaryTreeMachineModel<Pheet, BaseModelT>::get_memory_level() {
	return base.get_memory_level();
}

}

#endif /* BINARYTREEMACHINEMODEL_H_ */
