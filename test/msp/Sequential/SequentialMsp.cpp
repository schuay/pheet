#include "SequentialMsp.h"

#include <memory>

using namespace graph;
using namespace sp;

namespace pheet
{

SequentialMsp::
SequentialMsp(const Graph* graph,
              const Node* start)
	: graph(graph), start(start)
{
}

ShortestPaths*
SequentialMsp::
operator()()
{
	ShortestPaths* sp = new ShortestPaths();

	PathPtr init(new Path(start));
	m_queue.insert(init);

	while (!m_queue.empty()) {
		/* Retrieve our next optimal candidate path. */

		PathPtr p = m_queue.first(1)[0];
		const Node* head = p->head();

		/* We've expanded up to head. The path is therefore optimal and must
		 * be added to our global shortest paths. */

		sp->paths[head].push_back(p);

		/* For all outgoing edges <- head: */

		for (auto & e : head->out_edges()) {
			/* The following steps are abstracted into queue.insert():
			 *
			 * Follow the edge, resulting in path p and weight w.
			 * Compare it to existing path weights for target node in set:
			 * For all dominated paths p':
			 *     Remove p' from queue.
			 * If p is not dominated by any existing path to head:
			 *     Add p to queue.
			 */
			std::shared_ptr<Path> q(p->step(e));
			const Node* qhead = q->head();

			/* A (somewhat ugly) special case for paths which are dominated by
			 * already final paths stored in ShortestPaths. */

			bool dominated = false;
			for (const auto & final_path : sp->paths[qhead]) {
				if (dominates(final_path.get(), q.get())) {
					dominated = true;
					break;
				}
			}

			if (dominated) {
				continue;
			}

			m_queue.insert(q);
		}
	}

	return sp;
}

}
