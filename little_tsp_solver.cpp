#include "little_tsp_solver.hpp"

#include <limits>
#include <stack>

#include <cassert>

#include "graph.hpp"
#include "little_tsp_tree_node.hpp"
#include "path.hpp"

using std::stack;
using std::numeric_limits;

const int infinity{numeric_limits<int>::max()};

static bool EvaluateNode(const Graph& graph, TreeNode& node, 
		TreeNode& smallest, int upper_bound);

Path LittleTSPSolver::Solve(const Graph& graph) const {
	return Solve(graph, infinity);
}

// method to compute optimal TSP
Path LittleTSPSolver::Solve(const Graph& graph, int upper_bound) const {
	// case: 0 vertices
	if (graph.GetNumVertices() == 0) { return Path{}; }

	// case: 1 vertex
	else if (graph.GetNumVertices() == 1) {
		Path one;
		one.vertices = { 0 };
		return one;
	}

	// create the first node from the adjacency "cost" matrix
	TreeNode root{graph};

	// set up for the branching and bounding
	stack<TreeNode> nodes; 
	TreeNode smallest{graph};

	// add the first node
	if (EvaluateNode(graph, root, smallest, upper_bound)) { nodes.push(root); }
	
	// branch and bound, baby, branch and bound
	while (!nodes.empty()) {
		// set upper bound
		int upper_bound{smallest.GetLowerBound() < upper_bound ?
			smallest.GetLowerBound() : upper_bound};

		// get the root node and remove it from the stack
		root = nodes.top();
		nodes.pop();

		// two branches:
		// 1. Exclude the highest penalty, lowest cost edge
		// (if excluding it doesn't create a disconnected graph)
		if (root.HasExcludeBranch()) {
			TreeNode exclude{root, root.GetNextEdge(), false};
			if (EvaluateNode(graph, exclude, smallest, upper_bound)) {
				nodes.push(exclude);
			}
		}

		// 2. Include the highest penalty, lowest cost edge
		TreeNode include{root, root.GetNextEdge(), true};
		if (EvaluateNode(graph, include, smallest, upper_bound)) {
			nodes.push(include);
		}
	}
	
	// return the shortest path
	assert(smallest.GetLowerBound() <= upper_bound);
	return smallest.GetTSPPath(graph);
}

// evaluate a node by calculating its next Edge and lower bound
// return true if the lower bound is less than the smallest path length
bool EvaluateNode(const Graph& graph, TreeNode& node, TreeNode& smallest, 
		int upper_bound) {
	// evaluate the node, add it to the list only if its 
	// lower bound is strictly less than the upper bound
	if (node.CalcLBAndNextEdge(graph)) 
		{ return node.GetLowerBound() < upper_bound; }
	
	// otherwise, a complete TSP path was found
	// set the solution to smallest if it less than smallest's length
	if (node.GetLowerBound() < smallest.GetLowerBound()) { smallest = node; }
	return false;
}
