#ifndef LITTLE_TSP_TREE_NODE_H
#define LITTLE_TSP_TREE_NODE_H

#include <vector>
#include <iostream>

#include "matrix.hpp"

// forward declare
class CostMatrix;  // defined in the cpp file
class Graph;
class Path;

struct Edge { int u, v; };

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
	void AddExclude(const Edge& e);

	// methods to deal with infinite
	bool IsInfinite(int i, int j) const { return infinite_(i, j); };
	void SetInfinite(int i, int j) { infinite_.SetEntry(i, j, true); };

	// getters
	int GetLowerBound() const { return lower_bound_; };

	// branching methods
	bool HasExcludeBranch() const { return has_exclude_branch_; };
	Edge GetNextEdge() const { return next_edge_; };
	
	// branch determination methods
	// use the current list of includes to recalculate the lower bound and
	// mark unavailable rows/columns that have been visited
	void SetAvailAndLB(const Graph& graph, CostMatrix& info);
	// calculate the lower bound and the next edge, return true if a next edge
	// was found
	bool CalcLBAndNextEdge(const Graph& graph);
	Path GetTSPPath(const Graph& graph) const;
	
	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const TreeNode& p);

private:
	// edges that are being included 
	std::vector<Edge> include;

	// bit matrix corresponding to the matrix cells set to infinite
	// kept here instead of CostMatrix to keep track of excludes
	Matrix<bool> infinite_;

	// keeping track for future TreeNodes
	Edge next_edge_;
	bool found_lb_and_edge_;
	bool has_exclude_branch_;

	// the node's lower bound
	int lower_bound_;
};

#endif  // LITTLE_TSP_TREE_NODE_H
