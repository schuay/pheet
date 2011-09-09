/*
 * BasicLowerBound.h
 *
 *  Created on: 09.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef BASICLOWERBOUND_H_
#define BASICLOWERBOUND_H_

#include "../graph_helpers.h"

/*
 *
 */
namespace pheet {

class BasicLowerBound {
public:
	BasicLowerBound();
	~BasicLowerBound();

	size_t operator()(GraphVertex* graph, size_t size, size_t k, std::set<size_t> const& set1, std::set<size_t> const& set2);

	static char const name[];
};

}

#endif /* BASICLOWERBOUND_H_ */
