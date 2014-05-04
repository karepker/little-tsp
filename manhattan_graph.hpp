#ifndef MANHATTAN_GRAPH_H
#define MANHATTAN_GRAPH_H

#include <iosfwd>
#include <sstream>

#include "graph.hpp"

class ManhattanGraph : public Graph {
public:
	explicit Graph(std::istream& input = std::cin);

	// gets information about the graph
	int operator()(int row, int col) const override;
	int get_num_vertices() const override { return vertices_.size(); }

	// outputs size of wold, number of vertices, and then the graph itself
	std::stringstream describe() const override;

	friend std::ostream& operator<<(std::ostream& os, 
		const Graph& graph);

private:
	int world_size_;
	int num_vertices_;
	std::vector<Coordinate> vertices_;

	// helpers for traveling salesperson
	struct Path naiveTSPHelper(struct Path soFar, std::vector<bool> visited);
};


#endif  // MANHATTAN_GRAPH_H
