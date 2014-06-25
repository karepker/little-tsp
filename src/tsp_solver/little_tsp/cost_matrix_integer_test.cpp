#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"

#include "edge.hpp"
#include "util.hpp"

#include "gtest/gtest.h"

const int c2_value{1};
const int c5_value{2};

const Edge edge{0, 0};

class CostMatrixIntegerTest : public ::testing::Test {
public:
	CostMatrixIntegerTest() : c2{c2_value, edge}, c3{-1, edge}, c4{-1, edge}, 
			c5{c5_value, edge} {
		// set c3 as unavailable and infinite, set c4 as infinite
		c3.SetInfinite();
		c3.SetUnavailable();

		c4.SetInfinite();
	}

protected:
	CostMatrixInteger c1, c2, c3, c4, c5;
};

TEST_F(CostMatrixIntegerTest, Infinite) {
	// test finiteness
	EXPECT_THROW(c1.IsInfinite(), NotAvailableError);
	EXPECT_FALSE(c2.IsInfinite());
	EXPECT_THROW(c3.IsInfinite(), NotAvailableError);
	EXPECT_TRUE(c4.IsInfinite());

	// test setting infinity
	EXPECT_THROW(c1.SetInfinite(), NotAvailableError);
	c2.SetInfinite();
	EXPECT_THROW(c3.SetInfinite(), NotAvailableError);
	c4.SetInfinite();

	// test finiteness again
	EXPECT_THROW(c1.IsInfinite(), NotAvailableError);
	EXPECT_TRUE(c2.IsInfinite());
	EXPECT_THROW(c3.IsInfinite(), NotAvailableError);
	EXPECT_TRUE(c4.IsInfinite());
}

TEST_F(CostMatrixIntegerTest, Available) {
	// test finiteness
	EXPECT_FALSE(c1.IsAvailable());
	EXPECT_TRUE(c2.IsAvailable());
	EXPECT_FALSE(c3.IsAvailable());
	EXPECT_TRUE(c4.IsAvailable());

	// test setting infinity
	c1.SetUnavailable();
	c2.SetUnavailable();
	c3.SetUnavailable();
	c4.SetUnavailable();

	// test finiteness again
	EXPECT_FALSE(c1.IsAvailable());
	EXPECT_FALSE(c2.IsAvailable());
	EXPECT_FALSE(c3.IsAvailable());
	EXPECT_FALSE(c4.IsAvailable());
}

TEST_F(CostMatrixIntegerTest, Incrementer) {
	// normal increment
	c2 += c5;
	EXPECT_EQ(c2_value + c5_value, c2());

	// decrement with numbers that are not available
	EXPECT_THROW(c3 += c5, NotAvailableError);
	EXPECT_THROW(c4 += c1, NotAvailableError);

	// decrement with infinite numbers
	c4 += c2;
	EXPECT_TRUE(c4.IsInfinite());
	c5 += c4;
	EXPECT_TRUE(c5.IsInfinite());
}

TEST_F(CostMatrixIntegerTest, Decrementer) {
	// normal decrement
	c2 -= c5;
	EXPECT_EQ(c2_value - c5_value, c2());

	// decrement with numbers that are not available
	EXPECT_THROW(c3 -= c5, NotAvailableError);
	EXPECT_THROW(c4 -= c1, NotAvailableError);

	// decrement with infinite numbers
	c4 -= c2;
	EXPECT_TRUE(c4.IsInfinite());
	EXPECT_THROW(c5 -= c4, ImplementationError);
}
