#include "tsp_solver/factory.hpp"

#include <string>
#include <memory>

#include "tsp_solver/tsp_solver.hpp"
#include "tsp_solver/naive_tsp_solver.hpp"
#include "tsp_solver/little_tsp/solver.hpp"
#include "tsp_solver/fast_tsp_solver.hpp"
#include "util.hpp"

using std::string;
using std::unique_ptr;

const string naive_tsp_type_c{"naive"};
const string little_tsp_type_c{"little"};
const string fast_tsp_type_c{"fast"};

unique_ptr<TSPSolver> CreateTSPSolver(const string& type) {
	if (type == naive_tsp_type_c) 
		{ return unique_ptr<TSPSolver>{new NaiveTSPSolver{}}; }
	if (type == little_tsp_type_c) 
		{ return unique_ptr<TSPSolver>{new LittleTSPSolver{}}; }
	if (type == fast_tsp_type_c) 
		{ return unique_ptr<TSPSolver>{new FastTSPSolver{}}; }
	throw Error{"Not a valid TSP solver type!"};
}
