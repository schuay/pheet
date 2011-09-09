/*
 * BasicNextVertex.h
 *
 *  Created on: 09.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef BASICNEXTVERTEX_H_
#define BASICNEXTVERTEX_H_

#include "../graph_helpers.h"

/*
 *
 */
namespace pheet {

class BasicNextVertex {
public:
	BasicNextVertex();
	~BasicNextVertex();

	size_t operator()(GraphVertex* graph, size_t size, size_t k, std::set<size_t> const& set1, std::set<size_t> const& set2);

	static char const name[];
};

}

#endif /* BASICNEXTVERTEX_H_ */
