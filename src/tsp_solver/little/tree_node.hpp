#ifndef TSP_SOLVER_LITTLE_TSP_TREE_NODE_H
#define TSP_SOLVER_LITTLE_TSP_TREE_NODE_H

#include <iosfwd>
#include <vector>

#include "edge.hpp"
#include "matrix.hpp"

class CostMatrix;
struct CostMatrixZero;
class Graph;
class Path;

class TreeNode {
public:
	// some constructors
	explicit TreeNode(const Graph& costs);

	// Important methods
	// add included and excluded vertices
	void AddInclude(const Edge& e);
	void AddExclude(const Edge& e) { exclude_.push_back(e); }

	// getters
	int GetLowerBound() const { return lower_bound_; }

	// branching methods
	bool HasExcludeBranch() const { return has_exclude_branch_; }
	Edge GetNextEdge() const { return next_edge_; }

	// calculate the lower bound and the next edge, return true if a next edge
	// was found
	bool CalcLBAndNextEdge();
	Path GetTSPPath() const;

	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const TreeNode& p);

private:
	bool HandleBaseCase(const CostMatrix& cost_matrix,
			const std::vector<CostMatrixZero>& remaining_edges);
	int CalculateLowerBound() const;

	// the graph from which the tree node gets its weights
	const Graph* graph_ptr_;

	// edges that are being included and excluded
	std::vector<Edge> include_;
	std::vector<Edge> exclude_;

	// keeping track for future TreeNodes
	Edge next_edge_;
	bool has_exclude_branch_;

	// the node's lower bound
	int lower_bound_;
};

#endif  // TSP_SOLVER_LITTLE_TSP_TREE_NODE_H
