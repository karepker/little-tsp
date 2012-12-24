// INCLUDES
// STL
#include <limits>
// Project 
#include "graph.hpp"

using std::vector;
using std::numeric_limits;

// implementation of optimal traveling salesperson methods
struct Path Graph::naiveTSPHelper(struct Path soFar, vector<bool> visited)
{
	struct Path minPath;
	minPath.length = numeric_limits<unsigned int>::max();
	bool openVertex = false;

	// go to each unvisited vertex
	for(unsigned int i = 0; i < this->numVertices; ++i)
	{
		if(!visited[i])
		{
			// create a new path and update its attributes based on the next vertex
			struct Path add = soFar;

			// update add to contain the new vertex as part of the path
			add.vertices.push_back(i);
			add.length += this->operator()(i, 
				add.vertices[add.vertices.size() - 2]);

			// update the visited bitvector
			vector<bool> nextVisited(visited);
			nextVisited[i] = true;
			openVertex = true;

			// recursive step
			struct Path option = this->naiveTSPHelper(add, nextVisited);

			// update the minPath if appropriate
			if(option.length < minPath.length)
			{
				minPath = option;
			}
		}
	}

	// base case: no vertices left
	if(!openVertex)
	{
		soFar.length += this->operator()
			(soFar.vertices[soFar.vertices.size() - 1], 0);
		return soFar;
	}

	// otherwise, return the minPath
	return minPath;
}

struct Path Graph::naiveTSP()
{
	// if no vertices
	if(this->numVertices == 0)
	{
		return Path();
	}		
	// start at zero, recurse
	struct Path start;
	start.length = 0;
	start.vertices.push_back(0);

	// update the visited bitVector
	vector<bool> visited(this->numVertices, false);
	visited[0] = true;

	// recursive step
	struct Path minPath = this->naiveTSPHelper(start, visited);

	return minPath;
}


