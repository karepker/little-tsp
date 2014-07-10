#include "tsp_solver/tsp_solver.hpp"

#include "graph/graph.hpp"
#include "path.hpp"

// for testing
/*
bool TSPSolver::is_valid_path(const struct Path& p) const
{
	// calculate the distance from each edge, make sure sum is correct
	int length = 0;
	for (int i = 0; i < p.vertices.size(); ++i)
	{
		length += operator()(p.vertices[i], 
			p.vertices[(i + 1) % p.vertices.size()]);
	}

	return length == p.length;
} */

Path TSPSolver::Solve(const Graph& graph, int) const { return Solve(graph); }
