#ifndef __GRAPH__
#define __GRAPH__

// INCLUDES
// Project
#include "basicgraph.hpp"

class Graph
{
	unsigned int worldSize;
	unsigned int numVertices;
	AdjMat adjMat;

	// helpers for traveling salesperson
	struct Path naiveTSPHelper(struct Path soFar, std::vector<bool> visited);

public:
	explicit Graph(std::istream& input = std::cin);

	// getters and setters
	const unsigned int operator()(unsigned int row, unsigned int col) const
	{ return this->adjMat(row, col); };
	unsigned int operator()(unsigned int row, unsigned int col)
	{ return this->adjMat(row, col); };
	unsigned int getNumVertices() { return this->numVertices; };

	// TSP Methods
	struct Path naiveTSP(); // for testing
	struct Path fastTSP(bool useOpt); 
	struct Path optTSP();
	struct Path optTSP(struct Path& fast);

	// validation methods
	bool isValidPath(const struct Path& p) const;

	friend std::ostream& operator<<(std::ostream& os, 
		const Graph& graph);
};

#endif // __GRAPH__
