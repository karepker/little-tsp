#include "tsp_solver/factory.hpp"

#include <string>
#include <memory>
#include <unordered_set>

#include "tsp_solver/fast.hpp"
#include "tsp_solver/little/solver.hpp"
#include "tsp_solver/naive.hpp"
#include "tsp_solver/tsp_solver.hpp"
#include "util.hpp"

using std::string;
using std::unique_ptr;
using std::unordered_set;

const string naive_tsp_type_c{"naive"};
const string little_tsp_type_c{"little"};
const string fast_tsp_type_c{"fast"};
const unordered_set<string> valid_tsp_types{
	naive_tsp_type_c, little_tsp_type_c, fast_tsp_type_c};


bool IsValidTSPSolverType(const std::string& type)
{ return valid_tsp_types.find(type) != valid_tsp_types.end(); }


unique_ptr<TSPSolver> CreateTSPSolver(const string& type) {
	if (type == naive_tsp_type_c)
		{ return unique_ptr<TSPSolver>{new NaiveTSPSolver{}}; }
	if (type == little_tsp_type_c)
		{ return unique_ptr<TSPSolver>{new LittleTSPSolver{}}; }
	if (type == fast_tsp_type_c)
		{ return unique_ptr<TSPSolver>{new FastTSPSolver{}}; }
	throw Error{"Not a valid TSP solver type!"};
}
