#ifndef __SET_H
#define __SET_H


#include "test/msp/lib/ShortestPath/Path.h"

namespace pareto
{

/**
 * Inteface for pareto set operations: insert() and remove()
 */
class Set
{
public:

	/**
	 * Insert the set of elements into the pareto set.
	 *
	 * For all elements e in paths,
	 * 1) add e only if it is not dominated by any element already in the set
	 * 2) prune all elements of the set dominated by e.
	 *
	 * Note that since the current set contains pareto optima only,
	 * either 1) or 2) will be the case, but not both or none.
	 */
	virtual void insert(sp::Paths paths) = 0;

	//TODO: range arguments for bulk removal
	//void remove() = 0;


};

}

#endif /* __SET_H */
