#ifndef __LINKED_QUEUE_H
#define __LINKED_QUEUE_H

#include "Queue.h"

#include <unordered_map>
#include <unordered_set>

#include "Less.h"

namespace pareto
{

/**
 * A naive implementation of the pareto queue using linked lists.
 */
class LinkedQueue : public Queue
{
public:
	LinkedQueue();
	virtual ~LinkedQueue();

	std::vector<sp::PathPtr> first(const size_t n) override = 0;
	void insert(sp::PathPtr path) override;
	bool empty() const override;

protected:
	typedef struct elem_t {
		struct elem_t* prev, *next;
		sp::PathPtr path;
	} elem_t;

	typedef std::unordered_map<const graph::Node*, std::unordered_set<elem_t*>>
	        node_ptrs_t;

protected:
	elem_t* list_insert(sp::PathPtr path);
	void    list_erase(elem_t* elem);

protected:
	node_ptrs_t m_elems_by_head;
	elem_t* m_list;

	less dominates;
};

}

#endif /* __LINKED_QUEUE_H */
