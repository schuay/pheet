/*
 * GraphDual.h
 *
 *  Created on: Jul 1, 2012
 *      Author: cederman
 */

#ifndef GRAPHDUAL_H_
#define GRAPHDUAL_H_

#include "../init.h"
#include "pheet/misc/types.h"
#include <vector>

#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>

namespace pheet {

class GraphNode {
public:
	GraphNode** edges;
	size_t num_edges;
	bool spawned_hint;
	size_t taken;

	GraphNode()
	{

	}

/*	GraphNode(GraphNode* edges, size_t num_edges):edges(edges),num_edges(num_edges)
	{
		spawned_hint = false;
		taken = 0;
	}*/

	size_t getDegree()
	{
		size_t degree = 0;
		for(int i=0;i<num_edges;i++)
			if(!edges[i]->isTaken())
				degree++;
		return degree;
	}

	bool take()
	{
		if(taken)
			return true;
		return SIZET_CAS(&taken,0,1);
	}

	bool isTaken()
	{
		return taken;
	}

};


class GraphDual
{
	std::vector<GraphNode> nodes;

	void generate_uniform(size_t size, size_t seed, float p)
	{
		nodes = std::vector<GraphNode>(size);
		boost::mt19937 rng;
		rng.seed(seed);
	    boost::uniform_real<float> rnd_f(0.0, 1.0);

		std::vector<GraphNode*>* edges = new std::vector<GraphNode*>[size];
		for(size_t i = 0; i < size; ++i) {
			for(size_t j = i + 1; j < size; ++j) {
				if(rnd_f(rng) < p) {
					edges[i].push_back(&nodes[j]);
					edges[j].push_back(&nodes[i]);
				}
			}

			nodes[i].num_edges = edges[i].size();
			nodes[i].taken = 0;
			nodes[i].spawned_hint = false;
			if(edges[i].size() > 0) {
				nodes[i].edges = new GraphNode*[edges[i].size()];
				for(size_t j = 0; j < edges[i].size(); ++j) {
					nodes[i].edges[j] = edges[i][j];
				}
			}
			else {
				nodes[i].edges = 0;
			}
		}
		delete[] edges;

	}

public:

	GraphDual(size_t size, size_t seed, float p)
	{
		generate_uniform(size,seed,p);
	}

	size_t size()
	{
		return nodes.size();
	}

	GraphNode* operator[](size_t index)
	{
		return &nodes[index];
	}


};


}

#endif /* GRAPHDUAL_H_ */
