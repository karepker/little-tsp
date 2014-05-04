// INCLUDES
// STL
#include <limits>
#include <stack>
// Project
#include "pathinfo.hpp"
#include "graph.hpp"

using std::stack;
using std::numeric_limits;

namespace
{
	const unsigned int infinity = numeric_limits<unsigned int>::max();
}

// evaluate a node by calculating its next Edge and lower bound
// return true if the lower bound is less than the smallest path length
bool evaluateNode(const struct AdjMat& costs, PathInfo& node, 
	PathInfo& smallest, unsigned int upperBound) 
{
	try
	{
		// evaluate the node, add it to the list only if its 
		// lower bound is strictly less than the upper bound
		node.calcLBAndNextEdge(costs);
		return node.getLowerBound() < upperBound;
	}

	// set the solution to smallest if it less than smallest's length
	catch(NoNextEdge)
	{
		if(node.getLowerBound() < smallest.getLowerBound())
		{
			smallest = node;
		}
		return false;
	}
}

struct Path Graph::optTSP()
{
	// run fasttsp to get a upper bound on path
	struct Path fast = this->fastTSP(false);
	return this->optTSP(fast);
}

// method to compute optimal TSP
struct Path Graph::optTSP(Path& fast)
{
	// case: 0 vertices
	if(this->numVertices == 0)
	{
		return Path();
	}

	// case: 1 vertex
	else if(this->numVertices == 1)
	{
		Path one;
		one.vertices = { 0 };
		one.length = 0;
		return one;
	}

	// create the first node from the adjacency "cost" matrix
	PathInfo root(this->adjMat);

	// set up for the branching and bounding
	stack<PathInfo> nodes; 
	PathInfo smallest;

	// add the first node
	if(evaluateNode(this->adjMat, root, smallest, fast.length))
	{
		nodes.push(root);
	}
	
	// branch and bound, baby, branch and bound
	while(!nodes.empty())
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
		if(root.getBothBranches())
		{
			PathInfo exclude(root, root.getNextEdge(), false);
			if(evaluateNode(this->adjMat, exclude, smallest, upperBound))
			{
				nodes.push(exclude);
			}
		}

		// 2. Include the highest penalty, lowest cost edge
		PathInfo include(root, root.getNextEdge(), true);
		if(evaluateNode(this->adjMat, include, smallest, upperBound))
		{
			nodes.push(include);
		}
	}
	
	// return the shortest path
	return smallest.getLowerBound() < fast.length ? 
		smallest.getTSPPath(this->adjMat) : fast;
}
