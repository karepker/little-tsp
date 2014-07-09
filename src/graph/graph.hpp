#ifndef GRAPH_H
#define GRAPH_H

#include <iosfwd>

struct Edge;
struct EdgeCost;

/* A simple but sufficent base graph class to give to a TSP solver. */
class Graph {
public:
	virtual ~Graph() {}

	// getters and setters
	virtual const EdgeCost& operator()(int row, int col) const = 0;
	virtual EdgeCost& operator()(int row, int col) = 0;
	virtual const EdgeCost& operator()(const Edge& e) const = 0;
	virtual EdgeCost& operator()(const Edge& e) = 0;
	virtual int GetNumVertices() const = 0;

	// output information about the graph
	virtual std::string Describe() const = 0;
};

// simply calls describe which can be overridden by derived classes
std::ostream& operator<<(std::ostream& os, const Graph& graph);

#endif // GRAPH_H
