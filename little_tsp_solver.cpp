#include "little_tsp_solver.h"

#include <limits>
#include <stack>

#include "pathinfo.hpp"
#include "graph.hpp"

using std::stack;
using std::numeric_limits;

const unsigned int infinity = numeric_limits<unsigned int>::max();

static bool evaluate_node(const Graph& graph, PathInfo& node, 
		PathInfo& smallest, unsigned int upperBound) {

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
		one.length = 0;
		return one;
	}

	// create the first node from the adjacency "cost" matrix
	PathInfo root{graph};

	// set up for the branching and bounding
	stack<PathInfo> nodes; 
	PathInfo smallest;

	// add the first node
	if (evaluate_node(graph, root, smallest, fast.length)) { nodes.push(root); }
	
	// branch and bound, baby, branch and bound
	while (!nodes.empty())
	{
		// set upper bound
		unsigned int upperBound = smallest.getLowerBound() < fast.length ?
			smallest.getLowerBound() : fast.length;

		// get the root node and remove it from the stack
		root = nodes.top();
		nodes.pop();

		// two branches:
		// 1. Exclude the highest penalty, lowest cost edge
		// (if excluding it doesn't create a disconnected graph)
		if (root.getBothBranches())
		{
			PathInfo exclude(root, root.getNextEdge(), false);
			if (evaluate_node(graph, exclude, smallest, upperBound))
			{
				nodes.push(exclude);
			}
		}

		// 2. Include the highest penalty, lowest cost edge
		PathInfo include(root, root.getNextEdge(), true);
		if (evaludate_node(graph, include, smallest, upperBound)) {
			nodes.push(include);
		}
	}
	
	// return the shortest path
	if (smallest.getLowerBound() > fast.length) { return fast; }
	return smallest.getTSPPath(graph);
}

// evaluate a node by calculating its next Edge and lower bound
// return true if the lower bound is less than the smallest path length
bool evaluate_node(const Graph& graph, PathInfo& node, 
		PathInfo& smallest, unsigned int upperBound) {
	try
	{
		// evaluate the node, add it to the list only if its 
		// lower bound is strictly less than the upper bound
		node.calcLBAndNextEdge(graph);
		return node.getLowerBound() < upperBound;
	}

	// set the solution to smallest if it less than smallest's length
	catch(NoNextEdge)
	{
		if (node.getLowerBound() < smallest.getLowerBound())
		{
			smallest = node;
		}
		return false;
	}
}


