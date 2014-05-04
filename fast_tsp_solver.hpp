#ifndef FAST_TSP_SOLVER_H
#define FAST_TSP_SOLVER_H

#include "tsp_solver.hpp"

/* Solves the TSP using a heuristic that may not be optimal */
class FastTSPSolver : public TSPSolver {
public:
	FastTSPSolver() : TSPSolver{false} {}

	Path solve(const Graph&) const override;
};

#endif  // FAST_TSP_SOLVER_H
