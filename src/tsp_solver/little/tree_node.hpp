#ifndef TSP_SOLVER_LITTLE_TSP_TREE_NODE_H
#define TSP_SOLVER_LITTLE_TSP_TREE_NODE_H

#include <iosfwd>
#include <vector>

#include "graph/edge.hpp"
#include "matrix.hpp"

class CostMatrix;
struct CostMatrixZero;
class Graph;
class Path;

class TreeNode {
public:
	// some constructors
	explicit TreeNode(const Graph& costs);

	// create children for the include and exclude branches given the parent
	static TreeNode MakeIncludeChild(const TreeNode& parent);
	static TreeNode MakeExcludeChild(const TreeNode& parent);

	// Important methods
	// add included and excluded vertices
	void AddInclude(const Edge& e);
	void AddExclude(const Edge& e) { exclude_(e.u, e.v) = 1; }

	int GetLowerBound() const { return lower_bound_; }
	Path GetTSPPath() const;

	// branching methods
	bool HasExcludeBranch() const { return has_exclude_branch_; }
	Edge GetNextEdge() const { return next_edge_; }

	// calculate the lower bound and the next edge
	// if true is returned, next_edge_ holds the next edge for the tree
	// has_exclude_branch_ tells whether the tree can branch to the left
	bool CalcLBAndNextEdge();

	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const TreeNode& p);

private:
	// Function to handle the base case when CalcLBAnNextEdge is called and
	// there are only two edges left to add.
	bool HandleBaseCase(const std::vector<CostMatrixZero>& remaining_edges);

	// Helper method to calculate the lower bound from the edges included.
	int CalculateLowerBound() const;

	// Once we add an include edge or an exclude edge, we don't know if the
	// node has an exclude branch or what the next edge will be anymore, so we
	// reset next_edge_ and has_exclude_branch_.
	void ResetCalculatedState();

	// the graph from which the tree node gets its weights
	const Graph* graph_ptr_;

	// edges that are being included and excluded
	std::vector<Edge> include_;
	// not using bool because operator[] is slow, thanks Bjarne
	Matrix<int> exclude_;

	// keeping track for future TreeNodes
	Edge next_edge_;  // set as (-1, -1) before it has been calculated
	bool has_exclude_branch_;  // false before it has been calculated

	// the node's lower bound
	int lower_bound_;  // may not be updated until CalcLBAndNextEdge is called
};

#endif  // TSP_SOLVER_LITTLE_TSP_TREE_NODE_H
