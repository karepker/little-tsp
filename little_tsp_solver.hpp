#ifndef LITTLE_TSP_SOLVER_H
#define LITTLE_TSP_SOLVER_H

#include "tsp_solver.hpp"

/* Solves the TSP using the optimal algorithm described by Little et. al */
class LittleTSPSolver : public TSPSolver {
public:
	LittleTSPSolver() : TSPSolver{true} {}

	Path solve(const Graph&) const override;
	Path solve(const Graph&, Path heurstic) const override;
};

#endif  // LITTLE_TSP_SOLVER_H
