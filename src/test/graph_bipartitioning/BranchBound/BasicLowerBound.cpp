/*
 * BasicLowerBound.cpp
 *
 *  Created on: 09.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include "BasicLowerBound.h"

#include <set>

namespace pheet {

BasicLowerBound::BasicLowerBound() {

}

BasicLowerBound::~BasicLowerBound() {

}

size_t BasicLowerBound::operator()(GraphVertex* graph, size_t size, size_t k, std::set<size_t> const& set1, std::set<size_t> const& set2) {
	size_t lb = 0;
	for(std::set<size_t>::const_iterator it = set1.begin(); it != set1.end(); ++it) {
		size_t v = *it;
		for(size_t i = 0; i < graph[v].num_edges; ++i) {
			size_t el = graph[v].edges[i].target;
			if(set2.find(el) != set2.end()) {
				lb += graph[v].edges[i].weight;
			}
		}
	}
	for(std::set<size_t>::const_iterator it = set2.begin(); it != set2.end(); ++it) {
		size_t v = *it;
		for(size_t i = 0; i < graph[v].num_edges; ++i) {
			size_t el = graph[v].edges[i].target;
			if(set1.find(el) != set1.end()) {
				lb += graph[v].edges[i].weight;
			}
		}
	}
	return lb;
}

char const BasicLowerBound::name[] = "BasicLowerBound";

}
