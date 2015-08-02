#ifndef TSP_SOLVER_TSP_SOLVER_H
#define TSP_SOLVER_TSP_SOLVER_H

// forward declarations
class Graph;
struct Path;

/* Base class defining a TSP solver which includes a solve member function and
 * some information about the solution it produces
 */
class TSPSolver {
public:
	TSPSolver(bool optimal) : optimal_{optimal} {}
	~TSPSolver() {}

	// solve the TSP for the given graph
	virtual Path Solve(const Graph&) const = 0;

	// allows an upper bound to be passed in (useful for optimal solvers)
	// by default, just calls the Solve member function
	virtual Path Solve(const Graph&, int upper_bound) const;

	// get information about the TSP solver
	bool is_optimal() const { return optimal_; }

private:
	bool optimal_;
};

#endif  // TSP_SOLVER_TSP_SOLVER_H
