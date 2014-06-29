#ifndef LITTLE_TSP_COST_MATRIX_INTEGER_H
#define LITTLE_TSP_COST_MATRIX_INTEGER_H

#include "util.hpp"
#include "edge.hpp"

/** 
 * class used to store integers and their states in a cost matrix in Little's
 * TSP algorithm. An integer's availability corresponds to whether or not it has
 * been "crossed out" in the cost matrix. An integer's infinite status
 * corresponds to whether or not it should be regarded as infinite
 */
class CostMatrixInteger {
public:
	// default construction is non-available
	CostMatrixInteger();
	// creates an available, finite integer with the given value
	CostMatrixInteger(int value, Edge e);

	void SetInfinite() { infinite_ = true; }

	// returns the value, throws an error if not available
	int operator()() const;
	bool IsInfinite() const { return infinite_; }
	Edge GetEdge() const { return edge_; }

	// Overloaded increment and decrement. These try and follow the rules of
	// math when an integer is infinite, and throw an error if its not available
	CostMatrixInteger& operator+=(CostMatrixInteger incrementer);
	CostMatrixInteger& operator-=(CostMatrixInteger decrementer);

	bool operator<(CostMatrixInteger other) const;

private:
	int value_;
	bool infinite_;
	// the edge is needed just as often as its weight, so store it
	Edge edge_;
};

#endif  // LITTLE_TSP_COST_MATRIX_INTEGER_H
