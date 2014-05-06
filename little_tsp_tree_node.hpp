#ifndef LITTLE_TSP_TREE_NODE_H
#define LITTLE_TSP_TREE_NODE_H

#include <vector>
#include <iostream>

#include "matrix.hpp"

// exception classes
class NoNextEdge {};

// forward declare
class Graph;
class Path;
struct Edge;  // defined in private section

class TreeNode {
public:
	// some constructors
	TreeNode();
	TreeNode(const Graph& costs);
	// copy constructor
	TreeNode(const TreeNode& old, Edge e, bool inc);

	// Important methods
	// add included and excluded vertices
	void AddInclude(const Edge& e);
	void AddExclude(const Edge& e);

	// methods to deal with infinite
	bool IsInfinite(int i, int j) 
		{ return infinite_(i, j); };
	void Setinfinite(int i, int j) 
		{ infinite_.setEntry(i, j, true); }; 

	// getters
	int GetLowerBound() const { return lower_bound_; }; 

	// branching methods
	bool HasExcludeBranch() const { return has_exclude_branch_; };
	Edge GetNextEdge() const { return next_edge_; };
	
	// branch determination methods
	void SetAvailAndLB(const Graph& graph, CostMatrix& info);
	void CalcLBAndNextEdge(const AdjMat& c);
	Path GetTSPPath(const Graph& graph) const;
	
	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const TreeNode& p);

private:
	struct Edge { int u, v; };

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
