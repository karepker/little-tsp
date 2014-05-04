#ifndef TSP_SOLVER_H
#define TSP_SOLVER_H

// forward declare the Graph class
class Graph;

/* Base class defining a TSP solver which includes a solve member function and
 * some information about the solution it produces
 */
class TSPSolver {
public:
	~TSPSolver() {}

	// solve the TSP for the given graph
	virtual Path solve(const Graph&) const = 0;

	// allow solutions to be passed in as a heuristic 
	// (useful for optimal solvers)
	// by default, just calls the solve member function
	virtual Path solve(const Graph&, Path heurstic) const { solve(graph); }

	// get information about the TSP solver
	bool is_optimal() const { return optimal_; }

private:
	bool optimal_;
};

#endif  // TSP_SOLVER_H
