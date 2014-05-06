#ifndef GRAPH_H
#define GRAPH_H

#include <iosfwd>
#include <sstream>

/* A simple but sufficent base graph class to give to a TSP solver. */
class Graph {
public:
	virtual ~Graph() {}

	// getters and setters
	virtual int operator()(int row, int col) const = 0;
	virtual int GetNumVertices() const = 0;

	// output information about the graph
	virtual std::stringstream Describe() const = 0;

	// simply calls describe which can be overridden by derived classes
	friend std::ostream& operator<<(std::ostream& os, 
		const Graph& graph) { describe(); }
};

#endif // GRAPH_H
