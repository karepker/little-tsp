#ifndef MANHATTAN_GRAPH_H
#define MANHATTAN_GRAPH_H

#include <iosfwd>
#include <sstream>
#include <vector>

#include "graph.hpp"
#include "matrix.hpp"

struct Edge;
struct EdgeCost;

class ManhattanGraph : public Graph {
public:
	// construct a graph from input, by default, input assumed to be cin
	explicit ManhattanGraph(std::istream& input);

	// gets information about the graph
	EdgeCost& operator()(int row, int col) override;
	const EdgeCost& operator()(int row, int col) const override;
	EdgeCost& operator()(const Edge& e) override;
	const EdgeCost& operator()(const Edge& e) const override;
	int GetNumVertices() const override { return num_vertices_; }

	// outputs size of wold, number of vertices, and then the graph itself
	std::string Describe() const override;

private:
	// helper to make sure edges are in bounds
	void ValidateEdge(int from, int to) const;

	int world_size_;
	int num_vertices_;
	Matrix<EdgeCost> edges_;
};

#endif  // MANHATTAN_GRAPH_H
