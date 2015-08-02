#ifndef GRAPH_EDGE_COST_H
#define GRAPH_EDGE_COST_H

#include "graph/edge.hpp"
#include "util.hpp"

/**
 * class used to store integers and their states in a cost matrix in Little's
 * TSP algorithm. An integer's availability corresponds to whether or not it has
 * been "crossed out" in the cost matrix. An integer's infinite status
 * corresponds to whether or not it should be regarded as infinite
 */
class EdgeCost {
public:
	// default construction is non-available
	EdgeCost();
	// creates an available, finite integer with the given value
	EdgeCost(int value, Edge e);
	static EdgeCost Infinite();
	static EdgeCost Infinite(const Edge& e);

	void SetInfinite() { infinite_ = true; }

	// returns the value, throws an error if not available
	int operator()() const;
	bool IsInfinite() const { return infinite_; }
	Edge GetEdge() const { return edge_; }

	// Overloaded increment and decrement. These try and follow the rules of
	// math when an integer is infinite, and throw an error if its not available
	EdgeCost& operator+=(EdgeCost incrementer);
	EdgeCost& operator-=(EdgeCost decrementer);

	EdgeCost operator-(int other) const;

	bool operator<(EdgeCost other) const;

private:
	int value_;
	bool infinite_;
	// the edge is needed just as often as its weight, so store it
	Edge edge_;
};

#endif  // GRAPH_EDGE_COST_H
