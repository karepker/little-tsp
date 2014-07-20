#include "graph/mock.hpp"

#include <cassert>

#include <vector>

#include "graph/edge.hpp"
#include "graph/edge_cost.hpp"
#include "matrix.hpp"

using std::vector;

Matrix<EdgeCost> MakeEdgeCosts(const vector<int>& weights, int size) {
	assert((size * size) == int(weights.size()));

	vector<EdgeCost> costs;
	int row{0};
	int column{0};

	// make the edge costs for every integer
	for (int weight : weights) {
		costs.push_back(EdgeCost{weight, Edge{row, column}});
		column = (column + 1) % size;
		if (column == 0) { ++row; }
	}

	// return a matrix
	return Matrix<EdgeCost>{size, costs};
}
