#ifndef MANHATTAN_GRAPH_H
#define MANHATTAN_GRAPH_H

#include <iosfwd>
#include <sstream>
#include <vector>

#include "graph.hpp"

struct Coordinate;

class ManhattanGraph : public Graph {
public:
	// construct a graph from input, by default, input assumed to be cin
	explicit ManhattanGraph(std::istream& input);

	// gets information about the graph
	int operator()(int row, int col) const override;
	int operator()(const Edge& e) const override;
	int GetNumVertices() const override { return vertices_.size(); }

	// outputs size of wold, number of vertices, and then the graph itself
	std::string Describe() const override;

private:
	// helper for getting the edge weight
	int GetEdgeWeight(int from, int to) const;

	int world_size_;
	int num_vertices_;
	std::vector<Coordinate> vertices_;
};

#endif  // MANHATTAN_GRAPH_H
