#ifndef TSP_SOLVER_FAST_H
#define TSP_SOLVER_FAST_H

#include "tsp_solver/tsp_solver.hpp"

/* Solves the TSP using a heuristic that may not be optimal */
class FastTSPSolver : public TSPSolver {
public:
	FastTSPSolver() : TSPSolver{false} {}

	Path Solve(const Graph&) const override;
};

#endif  // TSP_SOLVER_FAST_H
