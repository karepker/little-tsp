#include "little_tsp_solver.h"

#include <limits>
#include <stack>

#include "TreeNode.hpp"
#include "graph.hpp"

using std::stack;
using std::numeric_limits;

const int infinity = numeric_limits<int>::max();

static bool EvaluateNode(const Graph& graph, TreeNode& node, 
		TreeNode& smallest, int upper_bound) {

Path LittleTSPSolver::solve(const Graph& graph)
{
	// TODO: fix this function
	return solve(graph, fast);
}

// method to compute optimal TSP
Path Graph::solve(const Graph& graph, Path fast)
{
	// case: 0 vertices
	if (graph.get_num_vertices() == 0) { return Path{}; }

	// case: 1 vertex
	else if (graph.get_num_vertices() == 1)
	{
		Path one;
		one.vertices = { 0 };
		return one;
	}

	// create the first node from the adjacency "cost" matrix
	TreeNode root{graph};

	// set up for the branching and bounding
	stack<TreeNode> nodes; 
	TreeNode smallest;

	// add the first node
	if (EvaluateNode(graph, root, smallest, fast.length)) { nodes.push(root); }
	
	// branch and bound, baby, branch and bound
	while (!nodes.empty())
	{
		// set upper bound
		int upper_bound{smallest.GetLowerBound() < fast.length ?
			smallest.GetLowerBound() : fast.length};

		// get the root node and remove it from the stack
		root = nodes.top();
		nodes.pop();

		// two branches:
		// 1. Exclude the highest penalty, lowest cost edge
		// (if excluding it doesn't create a disconnected graph)
		if (root.HasExcludeBranch())
		{
			TreeNode exclude{root, root.getNextEdge(), false};
			if (EvaluateNode(graph, exclude, smallest, upper_bound))
			{
				nodes.push(exclude);
			}
		}

		// 2. Include the highest penalty, lowest cost edge
		TreeNode include{root, root.getNextEdge(), true};
		if (EvaluateNode(graph, include, smallest, upper_bound)) {
			nodes.push(include);
		}
	}
	
	// return the shortest path
	if (smallest.GetLowerBound() > fast.length) { return fast; }
	return smallest.getTSPPath(graph);
}

// evaluate a node by calculating its next Edge and lower bound
// return true if the lower bound is less than the smallest path length
bool EvaluateNode(const Graph& graph, TreeNode& node, TreeNode& smallest, 
		int upper_bound) {
	try
	{
		// evaluate the node, add it to the list only if its 
		// lower bound is strictly less than the upper bound
		node.CalcLBAndNextEdge(graph);
		return node.GetLowerBound() < upper_bound;
	}

	// set the solution to smallest if it less than smallest's length
	catch(NoNextEdge)
	{
		if (node.GetLowerBound() < smallest.GetLowerBound()) 
		{ smallest = node; }
		return false;
	}
}


