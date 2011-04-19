/*
 * BinaryTreeCPUHierarchy.h
 *
 *  Created on: 18.04.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef BINARYTREECPUHIERARCHY_H_
#define BINARYTREECPUHIERARCHY_H_

/*
 *
 */
namespace pheet {

template <class BaseCPUHierarchy>
class BinaryTreeCPUHierarchy {
public:
	typedef typename BaseCPUHierarchy::CPUDescriptor CPUDescriptor;

	BinaryTreeCPUHierarchy(BaseCPUHierarchy* base);
	~BinaryTreeCPUHierarchy();

	procs_t get_size();
	vector<BinaryTreeCPUHierarchy<BaseCPUHierarchy>*> const* get_subsets();
	vector<CPUDescriptor*> const* get_cpus();

private:
	BinaryTreeCPUHierarchy(typename vector<BaseCPUHierarchy*>::const_iterator begin, typename vector<BaseCPUHierarchy*>::const_iterator end, procs_t size);

	void init_subsets(typename vector<BaseCPUHierarchy*>::const_iterator begin, typename vector<BaseCPUHierarchy*>::const_iterator end, procs_t size);

	BaseCPUHierarchy* base;
	vector<BinaryTreeCPUHierarchy*> subsets;

	procs_t size;
};

template <class BaseCPUHierarchy>
BinaryTreeCPUHierarchy<BaseCPUHierarchy>::BinaryTreeCPUHierarchy(BaseCPUHierarchy* base)
: base(base), size(base.get_size()) {

}

template <class BaseCPUHierarchy>
BinaryTreeCPUHierarchy<BaseCPUHierarchy>::BinaryTreeCPUHierarchy(typename vector<BaseCPUHierarchy*>::const_iterator begin, typename vector<BaseCPUHierarchy*>::const_iterator end, procs_t size)
: base(NULL), size(size) {
	init_subsets(begin, end, size);
}

template <class BaseCPUHierarchy>
BinaryTreeCPUHierarchy<BaseCPUHierarchy>::~BinaryTreeCPUHierarchy() {
	for(size_t i = 0; i < subsets.size(); i++) {
		delete subsets[i];
	}
}

template <class BaseCPUHierarchy>
procs_t BinaryTreeCPUHierarchy<BaseCPUHierarchy>::get_size () {
	return size;
}

template <class BaseCPUHierarchy>
vector<BinaryTreeCPUHierarchy<BaseCPUHierarchy>*> const* BinaryTreeCPUHierarchy<BaseCPUHierarchy>::get_subsets() {
	if(size > 1 && subsets.size() == 0) {
		vector<BaseCPUHierarchy*> const* sub = base->get_subsets();

		init_subsets(sub->begin(), sub->end(), sub->size());
	}
	return &subsets;
}

template <class BaseCPUHierarchy>
void BinaryTreeCPUHierarchy<BaseCPUHierarchy>::init_subsets(typename vector<BaseCPUHierarchy*>::const_iterator begin, typename vector<BaseCPUHierarchy*>::const_iterator end, procs_t size) {
	if(size > 2) {
		subsets.reserve(2);
		procs_t half = size / 2;
		subsets.push_back(new BinaryTreeCPUHierarchy(begin, begin + (size-half), size - half));
		subsets.push_back(new BinaryTreeCPUHierarchy(begin + (size-half), end, size));
	}
	else {
		subsets.reserve(size);
		for(typename vector<BaseCPUHierarchy*>::iterator i = begin; i != end; i++) {
			subsets.push_back(new BinaryTreeCPUHierarchy(*i));
		}
	}
}

}

#endif /* BINARYTREECPUHIERARCHY_H_ */
