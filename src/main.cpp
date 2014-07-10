////////////////////////////////////////////////////////////////////////////////
// Kar Epker's Implementation of Little et. al. TSP Algorithm
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <memory>

#include "graph/factory.hpp"
#include "graph/graph.hpp"
#include "interaction.hpp"
#include "path.hpp"
#include "tsp_solver/factory.hpp"
#include "tsp_solver/tsp_solver.hpp"
#include "util.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::unique_ptr;

int main(int argc, char* argv[]) {
	// get mode
	programmode_t mode;
	try { mode = parseArgs(argc, argv); }
	catch (Error& e) {
		cout << e.what() << endl;
		return 1;
	} catch (ImplementationError& ie) {
		cout << "Implementation Error: " << ie.what() << endl;
		return 2;
	} catch (...) {
		cout << "Unknown Error!" << endl;
		return 3;
	}

	unique_ptr<Graph> graph{CreateGraph("manhattan", cin)};
	unique_ptr<TSPSolver> tsp_solver;

	// do the calculations
	// find the optimal TSP using the faster algorithm
	if (mode == OPTTSP) { tsp_solver = CreateTSPSolver("little"); }
	// find the optimal TSP using the naive algorithm
	else { tsp_solver = CreateTSPSolver("naive"); }

	Path p{tsp_solver->Solve(*graph)};
	cout << p;

	return 0;
}
