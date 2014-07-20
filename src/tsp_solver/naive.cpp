#include "tsp_solver/naive.hpp"

#include <limits>
#include <vector>

#include "graph/edge_cost.hpp"
#include "graph/graph.hpp"
#include "path.hpp"

using std::vector;
using std::numeric_limits;

static Path SolveHelper(const Graph& graph, Path soFar, vector<bool> visited);

Path NaiveTSPSolver::Solve(const Graph& graph) const {
	// if no vertices
	if (!graph.GetNumVertices()) { return Path{}; }

	// start at zero, recurse
	Path start;
	start.length = 0;
	start.vertices.push_back(0);

	// update the visited bitVector
	vector<bool> visited(graph.GetNumVertices(), false);
	visited[0] = true;

	// recursive step
	Path minPath{SolveHelper(graph, start, visited)};
	return minPath;
}

// implementation of optimal traveling salesperson methods
Path SolveHelper(const Graph& graph, Path soFar, vector<bool> visited) {
	Path minPath;
	minPath.length = numeric_limits<int>::max();
	bool openVertex{false};

	// go to each unvisited vertex
	for (int i{0}; i < graph.GetNumVertices(); ++i) {
		if (!visited[i]) {
			// copy the current path and update it based on the next vertex
			Path add{soFar};

			// update add to contain the new vertex as part of the path
			add.vertices.push_back(i);
			add.length += graph(i, add.vertices[add.vertices.size() - 2])();

			// update the visited bitvector
			vector<bool> nextVisited{visited};
			nextVisited[i] = true;
			openVertex = true;

			// recursive step
			Path option{SolveHelper(graph, add, nextVisited)};

			// update the minPath if appropriate
			if (option.length < minPath.length) { minPath = option; }
		}
	}

	// base case: level reached where all vertices are already part of the path
	if (!openVertex) {
		soFar.length += graph(soFar.vertices[soFar.vertices.size() - 1], 0)();
		return soFar;
	}

	// at each level, once all possible path choices have been explored, return
	// the one with the minimum weight
	return minPath;
}
