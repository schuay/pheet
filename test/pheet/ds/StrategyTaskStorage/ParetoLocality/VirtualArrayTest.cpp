#include <gtest/gtest.h>

#include <pheet/ds/StrategyTaskStorage/ParetoLocality/VirtualArray.h>

using namespace pheet;

namespace
{

class VirtualArrayTest : public testing::Test
{
protected:
	virtual void SetUp()
	{
		xs.increase_capacity(3);
		xs[0] = &ys[0];
		xs[1] = &ys[1];
		xs[2] = &ys[2];

		zs.increase_capacity(zs.block_size());
		for (size_t i = 0; i < zs.block_size(); i++) {
			zs[i] = &ys[i % 3];
		}
	}

	int ys[3] = { 0, 1, 2 };
	VirtualArray<int*> xs;
	VirtualArray<int*> zs;
};

TEST_F(VirtualArrayTest, PushAndArrayGetter)
{
	EXPECT_EQ(&ys[0], xs[0]);
	EXPECT_EQ(&ys[1], xs[1]);
	EXPECT_EQ(&ys[2], xs[2]);
}

TEST_F(VirtualArrayTest, ArraySetterAndGetter)
{
	xs[0] = &ys[2];
	xs[1] = &ys[1];
	xs[2] = &ys[0];
	EXPECT_EQ(&ys[2], xs[0]);
	EXPECT_EQ(&ys[1], xs[1]);
	EXPECT_EQ(&ys[0], xs[2]);
}

TEST_F(VirtualArrayTest, PushAndSwap)
{
	xs.swap(0, 2);
	EXPECT_EQ(&ys[2], xs[0]);
	EXPECT_EQ(&ys[1], xs[1]);
	EXPECT_EQ(&ys[0], xs[2]);
}

TEST_F(VirtualArrayTest, IterTraversal)
{
	int i = 0;
	for (auto it = xs.begin(); it != xs.iterator_to(3); it++) {
		EXPECT_EQ(&ys[i++], *it);
	}
}

TEST_F(VirtualArrayTest, IterLessThan)
{
	int i = 0;
	for (auto it = xs.begin(); it < xs.iterator_to(3); it++) {
		EXPECT_EQ(&ys[i++], *it);
	}
}

TEST_F(VirtualArrayTest, IterEndWithFullBlock)
{
	int i = 0;
	for (auto it = zs.begin(); it != zs.iterator_to(3); it++) {
		EXPECT_EQ(&ys[i++ % 3], *it);
	}
}

TEST_F(VirtualArrayTest, IterPostIncrement)
{
	auto it1 = xs.begin();
	auto it0 = it1++;

	EXPECT_EQ(&ys[0], *it0);
	EXPECT_EQ(&ys[1], *it1);
}

TEST_F(VirtualArrayTest, IterReverseTraversal)
{
	/* TODO: This one is awkward to set up. We need iterator arithmetic
	 * and a >= operator. */
	int i = 2;
	auto it = xs.iterator_to(2);
	while (true) {
		EXPECT_EQ(&ys[i--], *it);
		if (it == xs.begin()) {
			break;
		}
		--it;
	}
}

TEST_F(VirtualArrayTest, IterIndex)
{
	auto it = zs.begin();
	EXPECT_EQ(0, it.index());
	it++;
	EXPECT_EQ(1, it.index());
	it = zs.end();
	EXPECT_EQ(zs.block_size(), it.index());

	zs.increase_capacity(1);
	zs[zs.block_size()] = &ys[0];
	it = zs.iterator_to(zs.block_size());
	EXPECT_EQ(zs.block_size(), it.index());
}

class IntWrapper
{
public:
	IntWrapper(int i) : i(i) { }
	int i;
};

TEST_F(VirtualArrayTest, IterDeref)
{
	constexpr int size = 42;
	VirtualArray<IntWrapper*> ints;
	ints.increase_capacity(42);
	for (int i = 0; i < size; i++) {
		ints[i] = new IntWrapper(i);
	}

	int i = 0;
	for (auto it = ints.begin(); it != ints.iterator_to(42); it++) {
		EXPECT_EQ(i++, it->i);
	}
}

TEST_F(VirtualArrayTest, IterSwap)
{
	auto l = xs.iterator_to(0);
	auto r = xs.iterator_to(2);

	std::swap(*l, *r);

	int i = 3;
	for (auto it = xs.begin(); it != xs.iterator_to(3); it++) {
		EXPECT_EQ(&ys[--i], *it);
	}
}

} /* namespace */

int
main(int argc,
     char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
