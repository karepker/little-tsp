#include "naive_tsp_solver.hpp"

#include <limits>
#include <vector>

#include "graph.hpp"
#include "path.hpp"

using std::vector;
using std::numeric_limits;

static Path solve_helper(const Graph& graph, Path soFar, vector<bool>& visited);

Path NaiveTSPSolver::solve(const Graph& graph) {
	// if no vertices
	if (this->numVertices == 0) { return Path(); }

	// start at zero, recurse
	Path start;
	start.length = 0;
	start.vertices.push_back(0);

	// update the visited bitVector
	vector<bool> visited(graph.get_num_vertices(), false);
	visited[0] = true;

	// recursive step
	struct Path minPath = this->naiveTSPHelper(start, visited);

	return minPath;
}

// implementation of optimal traveling salesperson methods
Path solve_helper(const Graph& graph, Path soFar, vector<bool> visited) {
	struct Path minPath;
	minPath.length = numeric_limits<unsigned int>::max();
	bool openVertex{false};

	// go to each unvisited vertex
	for (int i = 0; i < graph.get_num_vertices(); ++i)
	{
		if (!visited[i])
		{
			// create a new path and update its attributes based on the next vertex
			struct Path add{soFar};

			// update add to contain the new vertex as part of the path
			add.vertices.push_back(i);
			add.length += operator()(i, add.vertices[add.vertices.size() - 2]);

			// update the visited bitvector
			vector<bool> nextVisited{visited};
			nextVisited[i] = true;
			openVertex = true;

			// recursive step
			struct Path option = solve_helper(graph, add, nextVisited);

			// update the minPath if appropriate
			if (option.length < minPath.length) { minPath = option; }
		}
	}

	// base case: level reached where all vertices are already part of the path
	if (!openVertex)
	{
		soFar.length += operator() 
			(soFar.vertices[soFar.vertices.size() - 1], 0);
		return soFar;
	}

	// at each level, once all possible path choices have been explored, return
	// the one with the minimum weight
	return minPath;
}
