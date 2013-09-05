#include <gtest/gtest.h>
#include <stack>
#include <unordered_set>

#include "Sequential/SequentialMsp.h"
#include "Strategy/StrategyMspTask.h"
#include "lib/Generator/Generator.h"
#include "lib/Pareto/Less.h"
#include "pheet/pheet.h"
#include "pheet/sched/Synchroneous/SynchroneousScheduler.h"

using namespace graph;
using namespace pareto;
using namespace pheet;
using namespace sp;

/* A generic test suite for MSP algorithms, consisting of:
 *
 * GenericTest: The generic test class. Responsible for constructing the
 *		graph, running the algorithm, and cleaning up.
 * Typed test classes (SequentialTest, StrategyTest, ...): Used to abstract
 *		differences between types. This *could* theoretically be done by using
 *      more than one template parameter in GenericTest, but GTest is limited to
 *		one.
 * TEST_GRAPH: This macro actually simplifies setting up full test runs for
 *		different graph parameters.
 */

#define TESTCASE GenericTest
#define TEST_GRAPH(nodes, edges, seed) \
TYPED_TEST(TESTCASE, test_##nodes##_##edges##_##seed) \
{ \
	this->init(nodes, edges, seed); \
	test_full(this->g, this->start, this->sp); \
}

namespace
{

class SequentialTest
{
public:
	typedef Pheet::WithScheduler<SynchroneousScheduler> SyncPheet;

	SequentialTest(Graph const* graph,
	               PathPtr const path)
		: graph(graph), path(path) {
	}

	ShortestPaths*
	operator()() {
		MspPerformanceCounters<SyncPheet> pc;
		Sets q(graph);
		SequentialMsp<SyncPheet> msp(graph, path, &q, pc);
		msp();
		return q.shortest_paths();
	}

private:
	Graph const* graph;
	PathPtr const path;
};

class StrategyTest
{
public:
	typedef Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<DistKStrategyTaskStorage>
	DistKPheet;

	StrategyTest(Graph const* graph,
	             PathPtr const path)
		: graph(graph), path(path) {
	}

	ShortestPaths*
	operator()() {
		MspPerformanceCounters<DistKPheet> pc;
		Sets q(graph);
		{
			typename DistKPheet::Environment env;
			StrategyMspTask<DistKPheet> msp(graph, path, &q, pc);
			msp();
		}
		return q.shortest_paths();
	}

private:
	Graph const* graph;
	PathPtr const path;
};

typedef ::testing::Types<SequentialTest, StrategyTest> TestTypes;
TYPED_TEST_CASE(TESTCASE, TestTypes);

template <typename T>
class TESTCASE : public ::testing::Test
{
protected:
	virtual void init(const int nodes,
	                  const int edges,
	                  const int seed) {
		g = Generator::directed("g", nodes, edges, true,
		                        Generator::default_weights(), seed);
		ASSERT_NE(g, nullptr);

		start = g->nodes().front();
		ASSERT_NE(start, nullptr);

		PathPtr p(new Path(start));

		T gen(g, p);
		sp = gen();
	}

	virtual void TearDown() {
		delete sp;
		delete g;
	}

protected:
	Graph* g;
	Node* start;
	ShortestPaths* sp;
};

bool
is_optimal_path_set(const Paths& paths)
{
	less dominates;

	for (size_t i = 0; i < paths.size(); i++) {
		for (size_t j = i + 1; j < paths.size(); j++) {
			const Path* l = paths[i].get();
			const Path* r = paths[j].get();

			if (dominates(l, r) || dominates(r, l)) {
				l->print();
				r->print();
				return false;
			}
		}
	}

	return true;
}

std::unordered_set<const Node*>
reachable_nodes(const Node* start)
{
	std::unordered_set<const Node*> set = { start };

	std::stack<const Node*> stack;
	for (const Edge * e : start->out_edges()) {
		stack.push(e->head());
	}

	while (!stack.empty()) {
		const Node* n = stack.top();
		stack.pop();

		if (set.find(n) != set.end()) {
			continue;
		}

		set.insert(n);
		for (const Edge * e : n->out_edges()) {
			stack.push(e->head());
		}
	}

	return set;
}

/**
 * Tests whether every node set within the solution is a Pareto set
 * (= no path within the set is dominated by any other path within the set).
 */
void
test_optimal_paths_only(const ShortestPaths* sp)
{
	for (const auto & node_paths_pair : sp->paths) {
		EXPECT_TRUE(is_optimal_path_set(node_paths_pair.second));
	}
}

/**
 * Tests whether all nodes which are reachable from the start node
 * are present within the solution set.
 */
void
test_all_reachable_nodes_present(const Node* start,
                                 const ShortestPaths* sp)
{
	std::unordered_set<const Node*> reachables = reachable_nodes(start);

	for (const auto & node_paths_pair : sp->paths) {
		reachables.erase(node_paths_pair.first);
	}

	EXPECT_TRUE(reachables.empty());
}

bool
pathptr_lexic_less(PathPtr const& l,
                   PathPtr const& r)
{
	weight_vector_t const& wl = l->weight();
	weight_vector_t const& wr = r->weight();

	for (size_t i = 0; i < wl.size(); i++) {
		if (wl[i] < wr[i]) {
			return true;
		} else if (wl[i] > wr[i]) {
			return false;
		}
	}

	return false;
}

void
expect_paths_eq(Paths const& lhs,
                Paths const& rhs)
{
	EXPECT_EQ(lhs.size(), rhs.size());

	Paths l(lhs);
	Paths r(rhs);

	std::sort(l.begin(), l.end(), pathptr_lexic_less);
	std::sort(r.begin(), r.end(), pathptr_lexic_less);

	for (size_t i = 0; i < l.size(); i++) {
		PathPtr& lp = l[i];
		PathPtr& rp = r[i];

		EXPECT_EQ(lp->head(), rp->head());
		EXPECT_EQ(lp->tail(), rp->tail());
		EXPECT_EQ(lp->weight(), rp->weight());

		/* Edges must not necessarily be equal. When given a choice of several
		 * paths with equal weights, any path may be chosen. */
	}
}

/**
 * Tests whether the solution produced by the current algorithm is equivalent
 * to the solution produced by the sequential algorithm. Of course, this
 * assumes that the sequential algorithm is correct.
 */
void
test_against_sequential(const Graph* graph,
                        const Node* start,
                        const ShortestPaths* sp)
{
	SequentialTest seq(graph, PathPtr(new Path(start)));
	const ShortestPaths* ssp = seq();

	ASSERT_EQ(sp->paths.size(), ssp->paths.size());

	for (auto it = ssp->paths.cbegin(); it != ssp->paths.cend(); it++) {
		Node const* n = it->first;

		Paths const& spaths = it->second;
		Paths const& paths  = sp->paths.at(n);

		expect_paths_eq(spaths, paths);
	}

	delete ssp;
}

void
test_full(const Graph* graph,
          const Node* start,
          const ShortestPaths* sp)
{
	test_optimal_paths_only(sp);
	test_all_reachable_nodes_present(start, sp);
	test_against_sequential(graph, start, sp);
}

TYPED_TEST(TESTCASE, SanityCheck)
{
	this->init(50, 150, 42);
}

TEST_GRAPH(50, 150, 42)
TEST_GRAPH(50, 500, 51)
TEST_GRAPH(50, 1270, 33)
TEST_GRAPH(50, 1000, 123)
TEST_GRAPH(333, 1501, 2031)

/* TODO: Add other correctness criteria. */

} /* namespace */

int
main(int argc,
     char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
