////////////////////////////////////////////////////////////////////////////////
// Kar Epker's Implementation of Little et. al. TSP Algorithm
////////////////////////////////////////////////////////////////////////////////

// INCLUDES
// STL
#include <iostream>
// Project
#include "interaction.hpp"
#include "graph.hpp"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	// get mode
	programmode_t mode;
	try
	{
		mode = parseArgs(argc, argv);
	}
	catch(MessageError me)
	{
		cout << me;
		return 1;
	}

	// construct the Graph
	Graph g;

	// do the calculations
	// find the optimal TSP using the faster algorithm
	if(mode == OPTTSP)
	{
		Path p = g.optTSP();
		cout << p;
	}
	// find the optimal TSP using the naive algorithm
	else
	{
		Path p = g.naiveTSP();
		cout << p;
	}

	return 0;
}

