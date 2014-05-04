#include "tsp_solver.hpp"

// for testing
bool TSPSolver::is_valid_path(const struct Path& p) const
{
	// calculate the distance from each edge, make sure sum is correct
	unsigned int length = 0;
	for(unsigned int i = 0; i < p.vertices.size(); ++i)
	{
		length += this->operator()(p.vertices[i], 
			p.vertices[(i + 1) % p.vertices.size()]);
	}

	return length == p.length;
}


