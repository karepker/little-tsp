#include "graph/edge_cost.hpp"

#include "graph/edge.hpp"

#include "gtest/gtest.h"

const int c2_value{1};
const int c4_value{2};

const Edge edge{0, 0};

class EdgeCostTest : public ::testing::Test {
public:
	EdgeCostTest() : c2{c2_value, edge}, c3{-1, edge},
		c4{c4_value, edge} {
		// set c3 as unavailable and infinite, set c3 as infinite
		c3.SetInfinite();
	}

protected:
	EdgeCost c1, c2, c3, c4;
};

TEST_F(EdgeCostTest, Infinite) {
	// test finiteness
	EXPECT_FALSE(c1.IsInfinite());
	EXPECT_FALSE(c2.IsInfinite());
	EXPECT_TRUE(c3.IsInfinite());

	// test setting infinity
	c1.SetInfinite();
	c2.SetInfinite();
	c3.SetInfinite();

	// test finiteness again
	EXPECT_TRUE(c1.IsInfinite());
	EXPECT_TRUE(c2.IsInfinite());
	EXPECT_TRUE(c3.IsInfinite());
}

TEST_F(EdgeCostTest, Incrementer) {
	// normal increment
	c2 += c4;
	EXPECT_EQ(c2_value + c4_value, c2());

	// increment with infinite numbers
	c3 += c2;
	EXPECT_TRUE(c3.IsInfinite());
	c4 += c3;
	EXPECT_TRUE(c4.IsInfinite());
}

TEST_F(EdgeCostTest, Decrementer) {
	// normal decrement
	c2 -= c4;
	EXPECT_EQ(c2_value - c4_value, c2());

	// decrement with infinite numbers
	c3 -= c2;
	EXPECT_TRUE(c3.IsInfinite());
	EXPECT_THROW(c4 -= c3, ImplementationError);
}
