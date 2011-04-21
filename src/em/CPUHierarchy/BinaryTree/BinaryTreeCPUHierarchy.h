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
	BinaryTreeCPUHierarchy(typename vector<BaseCPUHierarchy*>::const_iterator begin, typename vector<BaseCPUHierarchy*>::const_iterator end);

	void init_subsets(typename vector<BaseCPUHierarchy*>::const_iterator begin, typename vector<BaseCPUHierarchy*>::const_iterator end);

	BaseCPUHierarchy* base;
	vector<BinaryTreeCPUHierarchy*> subsets;
	vector<CPUDescriptor*> cpus;

	procs_t size;
};

template <class BaseCPUHierarchy>
BinaryTreeCPUHierarchy<BaseCPUHierarchy>::BinaryTreeCPUHierarchy(BaseCPUHierarchy* base)
: base(base), size(base->get_size()) {

}

template <class BaseCPUHierarchy>
BinaryTreeCPUHierarchy<BaseCPUHierarchy>::BinaryTreeCPUHierarchy(typename vector<BaseCPUHierarchy*>::const_iterator begin, typename vector<BaseCPUHierarchy*>::const_iterator end)
: base(NULL), size(0) {
	for(typename vector<BaseCPUHierarchy*>::const_iterator i = begin; i != end; ++i) {
		size += (*i)->get_size();
	}
	init_subsets(begin, end);
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

		init_subsets(sub->begin(), sub->end());
	}
	return &subsets;
}

template <class BaseCPUHierarchy>
void BinaryTreeCPUHierarchy<BaseCPUHierarchy>::init_subsets(typename vector<BaseCPUHierarchy*>::const_iterator begin, typename vector<BaseCPUHierarchy*>::const_iterator end) {
	size_t subset_size = end - begin;
	if(subset_size > 2) {
		subsets.reserve(2);
		procs_t half = subset_size / 2;
		subsets.push_back(new BinaryTreeCPUHierarchy(begin, begin + (subset_size-half)));
		subsets.push_back(new BinaryTreeCPUHierarchy(begin + (subset_size-half), end));
	}
	else {
		subsets.reserve(subset_size);
		for(typename vector<BaseCPUHierarchy*>::const_iterator i = begin; i != end; ++i) {
			subsets.push_back(new BinaryTreeCPUHierarchy(*i));
		}
	}
}

template <class BaseCPUHierarchy>
vector<typename BaseCPUHierarchy::CPUDescriptor*> const* BinaryTreeCPUHierarchy<BaseCPUHierarchy>::get_cpus() {
	if(cpus.size() == 0) {
		if(base != NULL) {
			cpus = *(base->get_cpus());
		}
		else {
			vector<BinaryTreeCPUHierarchy<BaseCPUHierarchy>*> const* subsets = get_subsets();
			for(size_t i = 0; i < subsets->size(); i++) {
				vector<CPUDescriptor*> const* subset_cpus = (*subsets)[i]->get_cpus();
				for(size_t j = 0; j < subset_cpus->size(); j++) {
					cpus.push_back((*subset_cpus)[j]);
				}
			}
		}
	}
	return &cpus;
}

}

#endif /* BINARYTREECPUHIERARCHY_H_ */
