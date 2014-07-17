#ifndef GRAPH_MOCK_H
#define GRAPH_MOCK_H

#include "graph.hpp"

#include <vector>

#include "gmock/gmock.h"

struct Edge;
class EdgeCost;
template<typename T>
class Matrix;

class MockGraph : public Graph {
public:
	// getters and setters
	MOCK_CONST_METHOD2(Predicate, const EdgeCost&(int row, int col));
	MOCK_METHOD2(Predicate, EdgeCost&(int row, int col));
	MOCK_CONST_METHOD1(Predicate, const EdgeCost&(const Edge& e));
	MOCK_METHOD1(Predicate, EdgeCost&(const Edge& e));

	const EdgeCost& operator()(int row, int col) const override
	{ return Predicate(row, col); }
	const EdgeCost& operator()(const Edge& e) const override
	{ return Predicate(e); }

	EdgeCost& operator()(const Edge& e) override { return Predicate(e); }
	EdgeCost& operator()(int from, int to) override
	{ return Predicate(from, to); }

	MOCK_CONST_METHOD0(GetNumVertices, int());

	// output information about the graph
	MOCK_CONST_METHOD0(Describe, std::string());
};

Matrix<EdgeCost> MakeEdgeCosts(const std::vector<int>& weights, int size);

#endif  // GRAPH_MOCK_H
