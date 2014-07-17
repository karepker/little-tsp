#include "tree_node.hpp"

#include <vector>

#include "graph/edge_cost.hpp"
#include "graph/mock.hpp"
#include "matrix.hpp"
#include "path.hpp"
#include "util.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::vector;

using ::testing::Const;
using ::testing::Return;
using ::testing::ReturnRef;

// cost matrix given in source paper (infinities on diagonals replaced by 0s)
const Matrix<EdgeCost> graph_weights{MakeEdgeCosts({
	0, 27, 43, 16, 30, 26,
	7, 0, 16, 1, 30, 25,
	20, 13, 0, 35, 5, 0,
	21, 16, 25, 0, 18, 18,
	12, 46, 27, 48, 0, 5,
	23, 5, 5, 9, 5, 0}, 6)};

class TreeNodeTest : public ::testing::Test {
public:
	TreeNodeTest() {
		for (int i{0}; i < 6; ++i) {
			for (int j{0}; j < 6; ++j) {
				EXPECT_CALL(Const(graph), Predicate(i, j)).WillRepeatedly(
						ReturnRef(graph_weights(i, j)));
				EXPECT_CALL(Const(graph), Predicate(Edge{i, j})).WillRepeatedly(
						ReturnRef(graph_weights(i, j)));
			}
		}
		EXPECT_CALL(graph, GetNumVertices()).WillRepeatedly(Return(6));
	}

	TreeNode TestIncludeBranch(TreeNode& parent, Edge e, int lower_bound);
	TreeNode TestExcludeBranch(TreeNode& parent, Edge e, int lower_bound);

protected:
	MockGraph graph;
};

// Test the whole tree
/*               - all tours -
 *              /             \
 *             /               \
 *     not(0, 3)               (0, 3)
 *    /        \             not(3, 0)
 * not(5, 2)   (5, 2)	    /         \
 *             (2, 5)    not(1, 0)    (1, 0)
 *                                 not(3, 1)
 *                                /         \
 *                        not(4, 5)         (4, 5)
 *                                          not(5, 4)
 *                                         /         \
 *                                 not(2, 4)         (2, 4)
 *                                                 not(5, 2)
 *                                                /         \
 *                                        not(3, 2)         (3, 2)
 *                                                          (5, 1)
 *                                                          ^ optimal tour: 63
 */

TEST_F(TreeNodeTest, GetTSPPath) {
	TreeNode root{graph};
	root.AddInclude({0, 3});
	root.AddInclude({1, 0});
	root.AddInclude({4, 5});
	root.AddInclude({2, 4});
	root.AddInclude({3, 2});
	root.AddInclude({5, 1});

	Path tsp_path{root.GetTSPPath()};
	const vector<int> expected_path{0, 3, 2, 4, 5, 1};
	EXPECT_EQ(expected_path, tsp_path.vertices);
	EXPECT_EQ(63, tsp_path.length);
}

TEST_F(TreeNodeTest, CalcLBAndNextEdgeExclude) {
	TreeNode root{graph};
	TreeNode level1{TestExcludeBranch(root, {0, 3}, 48)};
	TestExcludeBranch(level1, {5, 2}, 58);
}

TEST_F(TreeNodeTest, CalcLBAndNextEdgeNoExclude) {
	Matrix<EdgeCost> weights{
		MakeEdgeCosts({-1, 1, 0, 0, -1, 2, 2, 0, -1}, 3)};
	MockGraph no_exclude_graph;
	for (int i{0}; i < 3; ++i) {
		for (int j{0}; j < 3; ++j) {
			EXPECT_CALL(Const(no_exclude_graph), Predicate(i, j)).
				WillRepeatedly(ReturnRef(weights(i, j)));
			EXPECT_CALL(Const(no_exclude_graph), Predicate(Edge{i, j})).
				WillRepeatedly(ReturnRef(weights(i, j)));
		}
	}
	EXPECT_CALL(no_exclude_graph, GetNumVertices()).WillRepeatedly(Return(3));

	// this looks like TestExcludeBranch
	TreeNode root{no_exclude_graph};
	Edge root_expected{1, 0};
	EXPECT_TRUE(root.CalcLBAndNextEdge());
	EXPECT_TRUE(root.HasExcludeBranch());
	EXPECT_EQ(root_expected, root.GetNextEdge());
	EXPECT_EQ(0, root.GetLowerBound());
	TreeNode level1{root};
	level1.AddExclude(root_expected);

	// here's the interesting part, level 1 can't have an exclude branch
	Edge level1_expected{1, 2};
	EXPECT_TRUE(level1.CalcLBAndNextEdge());
	EXPECT_FALSE(level1.HasExcludeBranch());
	EXPECT_EQ(level1_expected, level1.GetNextEdge());
	EXPECT_EQ(4, level1.GetLowerBound());
	TreeNode level2{root};
	level2.AddInclude(level1_expected);

	// check the base case:
	EXPECT_FALSE(level2.CalcLBAndNextEdge());
	EXPECT_FALSE(level2.HasExcludeBranch());
	EXPECT_EQ(5, level2.GetLowerBound());

	// check that the path is right
	Path tsp_path{level2.GetTSPPath()};
	const vector<int> expected_path{0, 1, 2};
	EXPECT_EQ(expected_path, tsp_path.vertices);
	EXPECT_EQ(5, tsp_path.length);
}

TEST_F(TreeNodeTest, IrreducibleNode) {
	// set up the graph
	const Matrix<EdgeCost> infinite_reduction{MakeEdgeCosts({
			-1, -1, 129, 86,
			39, -1, 112, 69,
			129, 112, -1, -1,
			86, 69, -1, -1}, 4)};
	MockGraph irreducible_graph;
	for (int i{0}; i < infinite_reduction.GetNumRows(); ++i) {
		for (int j{0}; j < infinite_reduction.GetNumColumns(); ++j) {
			EXPECT_CALL(Const(irreducible_graph), Predicate(i, j)).
				WillRepeatedly(ReturnRef(infinite_reduction(i, j)));
		}
	}
	EXPECT_CALL(irreducible_graph, GetNumVertices()).WillRepeatedly(Return(4));

	// set up the node
	TreeNode node{irreducible_graph};
	node.AddInclude({1, 0});
	node.AddInclude({0, 2});
	node.AddExclude({0, 1});
	node.AddExclude({2, 3});
	node.AddExclude({3, 2});

	// calculate the next edge, expect the return value to be false
	EXPECT_FALSE(node.CalcLBAndNextEdge());
	EXPECT_FALSE(node.HasExcludeBranch());
}

TEST_F(TreeNodeTest, InfiniteRowAndColumnPenalty) {
	const Matrix<EdgeCost> zeros_alone_costs{MakeEdgeCosts({
			-1, -1, 0,
			0, -1, -1,
			-1, 0, -1}, 3)};
	MockGraph zeros_alone;
	for (int i{0}; i < zeros_alone_costs.GetNumRows(); ++i) {
		for (int j{0}; j < zeros_alone_costs.GetNumColumns(); ++j) {
			EXPECT_CALL(Const(zeros_alone), Predicate(i, j)).
				WillRepeatedly(ReturnRef(zeros_alone_costs(i, j)));
		}
	}
	EXPECT_CALL(zeros_alone, GetNumVertices()).WillRepeatedly(Return(3));

	// set up the node
	TreeNode node{zeros_alone};
	node.AddExclude({0, 1});
	node.AddExclude({1, 2});
	node.AddExclude({2, 0});

	// calculate the next edge, expect no exclude branch
	Edge expected{0, 2};
	EXPECT_TRUE(node.CalcLBAndNextEdge());
	EXPECT_EQ(expected, node.GetNextEdge());
	EXPECT_FALSE(node.HasExcludeBranch());
}

TEST_F(TreeNodeTest, CalcLBAndNextEdge2by2Zeros) {
	Matrix<EdgeCost> weights{MakeEdgeCosts({-1, 0, 1, 0, -1, 0, 0, 1, -1}, 3)};
	MockGraph test_graph;
	for (int i{0}; i < 3; ++i) {
		for (int j{0}; j < 3; ++j) {
			EXPECT_CALL(Const(test_graph), Predicate(i, j)).WillRepeatedly(
					ReturnRef(weights(i, j)));
			EXPECT_CALL(Const(test_graph), Predicate(Edge{i, j})).
				WillRepeatedly(ReturnRef(weights(i, j)));
		}
	}
	EXPECT_CALL(test_graph, GetNumVertices()).WillRepeatedly(Return(3));

	// root
	TreeNode root{test_graph};
	Edge level1_expected{0, 1};
	EXPECT_TRUE(root.CalcLBAndNextEdge());
	EXPECT_TRUE(root.HasExcludeBranch());
	EXPECT_EQ(level1_expected, root.GetNextEdge());
	EXPECT_EQ(0, root.GetLowerBound());
	TreeNode level1{root};
	level1.AddInclude(level1_expected);

	// level 1 (last node)
	EXPECT_FALSE(level1.CalcLBAndNextEdge());
	EXPECT_EQ(0, level1.GetLowerBound());

	// check that the path is right
	Path tsp_path{level1.GetTSPPath()};
	const vector<int> expected_path{0, 1, 2};
	EXPECT_EQ(expected_path, tsp_path.vertices);
	EXPECT_EQ(0, tsp_path.length);
}

TEST_F(TreeNodeTest, CalcLBAndNextEdgeInclude) {
	TreeNode root{graph};
	TreeNode level1{TestIncludeBranch(root, {0, 3}, 48)};
	TreeNode level2{TestIncludeBranch(level1, {1, 0}, 49)};
	TreeNode level3{TestIncludeBranch(level2, {4, 5}, 51)};
	TreeNode end{TestIncludeBranch(level3, {2, 4}, 56)};

	// test the base case
	EXPECT_FALSE(end.CalcLBAndNextEdge());
	EXPECT_FALSE(end.HasExcludeBranch());
	EXPECT_EQ(63, end.GetLowerBound());

	// make sure the base case added the right vertices
	Path tsp_path{end.GetTSPPath()};
	const vector<int> expected_path{0, 3, 2, 4, 5, 1};
	EXPECT_EQ(expected_path, tsp_path.vertices);
	EXPECT_EQ(63, tsp_path.length);
}

TreeNode TreeNodeTest::TestIncludeBranch(TreeNode& parent, Edge e,
		int lower_bound) {
	EXPECT_TRUE(parent.CalcLBAndNextEdge());
	EXPECT_TRUE(parent.HasExcludeBranch());
	EXPECT_EQ(e, parent.GetNextEdge());
	EXPECT_EQ(lower_bound, parent.GetLowerBound());
	TreeNode node{parent};
	node.AddInclude(e);
	return node;
}

TreeNode TreeNodeTest::TestExcludeBranch(TreeNode& parent, Edge e,
		int lower_bound) {
	EXPECT_TRUE(parent.CalcLBAndNextEdge());
	EXPECT_TRUE(parent.HasExcludeBranch());
	EXPECT_EQ(e, parent.GetNextEdge());
	EXPECT_EQ(lower_bound, parent.GetLowerBound());
	TreeNode node{parent};
	node.AddExclude(e);
	return node;
}
