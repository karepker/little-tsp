#ifndef GRAPH_MOCK_H
#define GRAPH_MOCK_H

#include "graph.hpp"

#include "gmock/gmock.h"

struct Edge;

class MockGraph : public Graph {
public:
	// getters and setters
	MOCK_CONST_METHOD2(Predicate, int(int row, int col));
	MOCK_CONST_METHOD1(Predicate, int(const Edge& e));
	int operator()(int row, int col) const override {
		return Predicate(row, col); 
	}
	int operator()(const Edge& e) const override { return Predicate(e); }

	MOCK_CONST_METHOD0(GetNumVertices, int());

	// output information about the graph
	MOCK_CONST_METHOD0(Describe, std::string());
};

#endif  // GRAPH_MOCK_H

