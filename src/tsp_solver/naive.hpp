#ifndef TSP_SOLVER_NAIVE_H
#define TSP_SOLVER_NAIVE_H

#include "tsp_solver/tsp_solver.hpp"

/* Solves the TSP using a naive optimal solution */
class NaiveTSPSolver : public TSPSolver {
public:
	NaiveTSPSolver() : TSPSolver{true} {}

	Path Solve(const Graph&) const override;
};

#endif  // TSP_SOLVER_NAIVE_H
