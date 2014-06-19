#ifndef LITTLE_TSP_TREE_NODE_H
#define LITTLE_TSP_TREE_NODE_H

#include <vector>
#include <iostream>

#include "matrix.hpp"
#include "util.hpp"

// forward declare
class CostMatrix;
struct CostMatrixZero;
struct Edge;
class Graph;
class Path;

class TreeNode {
public:
	// some constructors
	TreeNode();
	explicit TreeNode(const Graph& costs);
	// copy constructor
	TreeNode(const TreeNode& old, Edge e, bool inc);

	// Important methods
	// add included and excluded vertices
	void AddInclude(const Edge& e);
	void AddExclude(const Edge& e) { exclude_.push_back(e); }

	// getters
	int GetLowerBound() const { return lower_bound_; };

	// branching methods
	bool HasExcludeBranch() const { return has_exclude_branch_; };
	Edge GetNextEdge() const { return next_edge_; };
	
	// calculate the lower bound and the next edge, return true if a next edge
	// was found
	bool CalcLBAndNextEdge(const Graph& graph);
	Path GetTSPPath(const Graph& graph) const;
	
	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const TreeNode& p);

private:
	bool HandleBaseCase(const Graph& graph, const CostMatrix& cost_matrix,
			const CostMatrixZero& zero);

	// edges that are being included and excluded
	std::vector<Edge> include_;
	std::vector<Edge> exclude_;

	// keeping track for future TreeNodes
	Edge next_edge_;
	bool found_lb_and_edge_;
	bool has_exclude_branch_;

	// the node's lower bound
	int lower_bound_;
};

#endif  // LITTLE_TSP_TREE_NODE_H
