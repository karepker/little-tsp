// INCLUDES
// STL
#include <fstream>
// Project
#include "graph.hpp"
// Google Test
#include "gtest/gtest.h"
	
using std::ifstream;

TEST(NaiveTSP, Case1)
{
	// get the map and make the graph
	ifstream cakes("test/test-1.txt");
	Graph g(cakes);

	// get the MST
	struct Path opt = g.naiveTSP();

	// run the tests: edges should have good sum and sum should be correct
	struct Path expected;
	expected.length = 32;
	expected.vertices = { 0, 1, 2, 3, 5, 4 };
	
	EXPECT_TRUE(g.isValidPath(opt));
	EXPECT_EQ(expected, opt);
}

TEST(NaiveTSP, Case2)
{
	// get the map and make the graph
	ifstream cakes("test/test-2.txt");
	Graph g(cakes);

	// get the MST
	struct Path opt = g.naiveTSP();

	// run the tests: edges should have good sum and sum should be correct
	struct Path expected;
	expected.length = 60;
	expected.vertices = { 0, 2, 4, 1, 3, 6, 5 };
	
	EXPECT_TRUE(g.isValidPath(opt));
	EXPECT_EQ(expected, opt);
}

TEST(NaiveTSP, Case3)
{
	// get the map and make the graph
	ifstream cakes("test/test-3.txt");
	Graph g(cakes);

	// get the MST
	struct Path opt = g.naiveTSP();

	// run the tests: edges should have good sum and sum should be correct
	struct Path expected;
	expected.length = 90;
	expected.vertices = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

	EXPECT_TRUE(g.isValidPath(opt));
	EXPECT_EQ(expected, opt);
}

TEST(NaiveTSP, Case4)
{
	// get the map and make the graph
	ifstream cakes("test/test-4.txt");
	Graph g(cakes);

	// get the MST
	struct Path opt = g.naiveTSP();

	// run the tests: edges should have good sum and sum should be correct
	struct Path expected;
	expected.length = 48;
	expected.vertices = { 0, 1, 2, 3, 4, 5 };

	EXPECT_TRUE(g.isValidPath(opt));
	EXPECT_EQ(expected, opt);
}
