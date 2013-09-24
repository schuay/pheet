/*
 * KLSMLocalityTaskStorageBlock.h
 *
 *  Created on: Sep 24, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGEBLOCK_H_
#define KLSMLOCALITYTASKSTORAGEBLOCK_H_

namespace pheet {

template <class Pheet, typename Item>
class KLSMLocalityTaskStorageBlock {
public:
	typedef KLSMLocalityTaskStorageBlock<Pheet, Item> Self;

	KLSMLocalityTaskStorageBlock(size_t size)
	:size(size) {
		data = new Item[size];
	}
	~KLSMLocalityTaskStorageBlock() {
		delete[] data;
	}

private:
	Item* data;
	size_t filled;
	size_t size;

	Self* next;

	size_t max_k;
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEBLOCK_H_ */
