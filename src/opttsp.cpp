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

bool evaluateNode(struct CostMatrix& costs, PathInfo& node, 
	PathInfo& smallest, unsigned int upperBound) 
{
	try
	{
		// evaluate the node, add it to the list only if its 
		// lower bound is strictly less than the upper bound
		costs.calcLBAndNextEdge(node);
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
	struct Path fast = this->fastTSP();
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

	// reduce the adjmat into a cost matrix, get lower bound
	const CostMatrix costs(this->adjMat);
	PathInfo root(this->adjMat);

	// set up for the branching and bounding
	stack<PathInfo> nodes; 
	PathInfo smallest;

	// add the first node
	if(evaluateNode(costs, root, smallest, fast.length))
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
		// 1. Excluding the highest penalty, lowest cost edge
		// (if excluding it doesn't create a disconnected graph)
		if(root.getBothBranches())
		{
			PathInfo exclude(root, root.getNextEdge(), false);
			if(evaluateNode(costs, exclude, smallest, upperBound))
			{
				nodes.push(exclude);
			}
		}

		// 2. Including the highest penalty, lowest cost edge
		PathInfo include(root, root.getNextEdge(), true);
		if(evaluateNode(costs, include, smallest, upperBound))
		{
			nodes.push(include);
		}
	}
	
	// return either fast or smallest, whichever has a smaller length
	return smallest.getLowerBound() < fast.length ? 
		costs.getTSPPath(smallest) : fast;
}
