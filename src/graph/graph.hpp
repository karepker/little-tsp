#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

#include <iosfwd>
#include <sstream>

struct Edge;

/* A simple but sufficent base graph class to give to a TSP solver. */
class Graph {
public:
	virtual ~Graph() {}

	// getters and setters
	virtual int operator()(int row, int col) const = 0;
	virtual int operator()(Edge e) const = 0;
	virtual int GetNumVertices() const = 0;

	// output information about the graph
	virtual std::string Describe() const = 0;
};

// simply calls describe which can be overridden by derived classes
std::ostream& operator<<(std::ostream& os, const Graph& graph);

#endif  // GRAPH_GRAPH_H
