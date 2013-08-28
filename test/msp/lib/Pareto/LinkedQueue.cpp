#include "LinkedQueue.h"

#include <vector>

using namespace graph;
using namespace sp;

namespace pareto
{

LinkedQueue::
LinkedQueue()
	: m_list(nullptr)
{
}

LinkedQueue::
~LinkedQueue()
{
	elem_t* curr = nullptr;
	elem_t* next = m_list;

	while (next != nullptr) {
		curr = next;
		next = next->next;
		delete curr;
	}
}

void
LinkedQueue::
insert(PathPtr path)
{
	const Node* head = path->head();
	std::unordered_set<elem_t*>& elems = m_elems_by_head[head];

	/* First, insert the new element. */

	elem_t* elem = list_insert(path);
	elems.insert(elem);

	/* Then, prune all dominated elements within this node's path set. */

	for (auto it = elems.begin(); it != elems.end();) {
		elem_t* other = *it;

		if (other == elem) {
			it++;
			continue;
		}

		if (dominates(elem->path.get(), other->path.get())) {
			/* Remove other. */
			elems.erase(it++);
			list_erase(other);
		} else if (dominates(other->path.get(), elem->path.get())) {
			/* Remove elem. Break from loop, since we only insert a single new
			 * element, domination is transitive, and within the original
			 * queue no paths dominate each other within a node set. */
			elems.erase(elem);
			list_erase(elem);
			return;
		} else { /* No domination in either direction. */
			it++;
		}
	}
}

bool
LinkedQueue::
empty() const
{
	return (m_list == nullptr);
}

LinkedQueue::elem_t*
LinkedQueue::
list_insert(PathPtr path)
{
	elem_t* elem = new elem_t;
	elem->path = path;
	elem->prev = nullptr;
	elem->next = m_list;

	if (m_list != nullptr) {
		m_list->prev = elem;
	}
	m_list = elem;

	return elem;
}

void
LinkedQueue::
list_erase(elem_t* elem)
{
	if (elem == m_list) {
		m_list = elem->next;
	}

	if (elem->prev != nullptr) {
		elem->prev->next = elem->next;
	}

	if (elem->next != nullptr) {
		elem->next->prev = elem->prev;
	}

	delete elem;
}

}
