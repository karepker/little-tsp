#ifndef TSP_SOLVER_LITTLE_SOLVER_H
#define TSP_SOLVER_LITTLE_SOLVER_H

#include "tsp_solver/tsp_solver.hpp"

/* Solves the TSP using the optimal algorithm described by Little et. al */
class LittleTSPSolver : public TSPSolver {
public:
	LittleTSPSolver() : TSPSolver{true} {}

	Path Solve(const Graph&) const override;
	Path Solve(const Graph&, int upper_bound) const override;
};

#endif  // TSP_SOLVER_LITTLE_SOLVER_H

