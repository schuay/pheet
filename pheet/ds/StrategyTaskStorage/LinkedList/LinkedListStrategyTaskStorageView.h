/*
 * LinkedListStrategyTaskStorageView.h
 *
 *  Created on: 07.04.2012
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef LINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_
#define LINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_

#include <vector>
#include "../../../misc/atomics.h"

namespace pheet {

class LinkedListStrategyTaskStorageView {
public:
	LinkedListStrategyTaskStorageView();
	~LinkedListStrategyTaskStorageView();

	void clean();

	bool try_register();
	void deregister();

	bool is_reusable() { return created_blocks == 0; }

	bool try_lock();
private:
	Self* prev;
	Self* next;

	size_t created_blocks;
	std::vector<DataBlock*> freed_blocks;

	ptrdiff_t reg;
};

LinkedListStrategyTaskStorageView::LinkedListStrategyTaskStorageView()
:prev(nullptr), next(nullptr), created_blocks(0), reg(0) {

}

LinkedListStrategyTaskStorageView::~LinkedListStrategyTaskStorageView() {
	pheet_assert(reg <= 0);

	for(auto i = freed_blocks.begin(); i != freed_blocks.end(); ++i) {
		delete *i;
	}
}

bool LinkedListStrategyTaskStorageView::try_lock() {
	if(reg == 0) {
		if(PTRDIFFT_CAS(&reg, 0, -1)) {
			return true;
		}
	}
	return reg == -1;
}

void LinkedListStrategyTaskStorageView::clean(std::vector<Self*>& view_reuse) {
	pheet_assert(reg == -1);
	for(auto i = freed_blocks.begin(); i != freed_blocks.end(); ++i) {
		if((*i)->first_view == this) {
			--created_blocks;
			delete *i;
		}
		else {
			pheet_assert(prev != nullptr);
			prev->freed_blocks.push_back(*i);
		}
	}
	if(prev != nullptr && prev->reg == -1) {
		prev->clean(view_reuse);
		if(created_blocks == 0) {
			prev->next = next;
		}
	}
	pheet_assert(next != nullptr);
	if(created_blocks == 0) {
		next->prev = prev;
		view_reuse.push_back(this);
	}
}

}

#endif /* LINKEDLISTSTRATEGYTASKSTORAGEVIEW_H_ */
