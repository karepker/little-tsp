#include "tree_node.hpp"

#include <vector>

#include "graph/mock.hpp"
#include "matrix.hpp"
#include "path.hpp"
#include "util.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::vector;

using ::testing::Return;

// cost matrix given in source paper (infinities on diagonals replaced by 0s)
const Matrix<int> graph_weights{6, {
	0, 27, 43, 16, 30, 26,
	7, 0, 16, 1, 30, 25,
	20, 13, 0, 35, 5, 0,
	21, 16, 25, 0, 18, 18,
	12, 46, 27, 48, 0, 5,
	23, 5, 5, 9, 5, 0}};

class TreeNodeTest : public ::testing::Test {
public:
	TreeNodeTest() {
		for (int i{0}; i < 6; ++i) {
			for (int j{0}; j < 6; ++j) {
				EXPECT_CALL(graph, Predicate(i, j)).WillRepeatedly(
						Return(graph_weights(i, j)));
			}
		}
		EXPECT_CALL(graph, GetNumVertices()).WillRepeatedly(Return(6));
	}

	TreeNode TestIncludeBranch(TreeNode& parent, Edge e);

protected:
	TreeNode tree_node1, tree_node2;
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
 *                                                          ^ optimal tour: 52
 */

TEST_F(TreeNodeTest, GetTSPPath) {
	TreeNode root{graph};
	root.AddInclude({0, 3});
	root.AddInclude({1, 0});
	root.AddInclude({4, 5});
	root.AddInclude({2, 4});
	root.AddInclude({3, 2});
	root.AddInclude({5, 1});

	Path tsp_path{root.GetTSPPath(graph)};
	const vector<int> expected_path{0, 3, 2, 4, 5, 1};
	EXPECT_EQ(expected_path, tsp_path.vertices);
	EXPECT_EQ(63, tsp_path.length);
}

TEST_F(TreeNodeTest, CalcLBAndNextEdge) {
	TreeNode root{graph};
	TreeNode level1{TestIncludeBranch(root, {0, 3})};
	TreeNode level2{TestIncludeBranch(level1, {1, 0})};
	TreeNode level3{TestIncludeBranch(level2, {4, 5})};
	TreeNode level4{TestIncludeBranch(level3, {2, 4})};

	// test the base case
	TreeNode end{level4};
	EXPECT_FALSE(end.CalcLBAndNextEdge(graph));
	EXPECT_FALSE(end.HasExcludeBranch());

	// make sure the base case added the right vertices
	Path tsp_path{root.GetTSPPath(graph)};
	const vector<int> expected_path{0, 3, 2, 4, 5, 1};
	EXPECT_EQ(expected_path, tsp_path.vertices);
	EXPECT_EQ(63, tsp_path.length);
};

TreeNode TreeNodeTest::TestIncludeBranch(TreeNode& parent, Edge e) {
	TreeNode node{parent};
	EXPECT_TRUE(node.CalcLBAndNextEdge(graph));
	EXPECT_TRUE(node.HasExcludeBranch());
	EXPECT_EQ(e, node.GetNextEdge());
	node.AddInclude(e);
	return node;
}
