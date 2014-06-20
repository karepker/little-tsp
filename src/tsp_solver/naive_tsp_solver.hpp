#ifndef NAIVE_TSP_SOLVER_H
#define NAIVE_TSP_SOLVER_H

#include "tsp_solver/tsp_solver.hpp"

/* Solves the TSP using a naive optimal solution */
class NaiveTSPSolver : public TSPSolver {
public:
	NaiveTSPSolver() : TSPSolver{true} {}

	Path Solve(const Graph&) const override;
};

#endif  // NAIVE_TSP_SOLVER_H
