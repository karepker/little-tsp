#ifndef LITTLE_TSP_COST_MATRIX_INTEGER_H
#define LITTLE_TSP_COST_MATRIX_INTEGER_H

#include "util.hpp"

/** 
 * class used to store integers and their states in a cost matrix in Little's
 * TSP algorithm. An integer's availability corresponds to whether or not it has
 * been "crossed out" in the cost matrix. An integer's infinite status
 * corresponds to whether or not it should be regarded as infinite
 */
class CostMatrixInteger {
public:
	CostMatrixInteger(int value);
	CostMatrixInteger(bool infinite, bool available);
	CostMatrixInteger(int value, bool infinite, bool available);

	void SetInfinite() { infinite_ = true; }
	void SetUnavailable() { available_ = false; }

	int operator()() const;
	bool IsAvailable() const { return available_; }
	bool IsInfinite() const { return infinite_; }

	// A whole bunch of overloaded operators. These try and follow the rules of
	// math when an integer is infinite, and throw an error if its not available
	CostMatrixInteger operator-(CostMatrixInteger other) const;
	CostMatrixInteger operator+(CostMatrixInteger other) const;

	CostMatrixInteger& operator+=(CostMatrixInteger incrementer);
	CostMatrixInteger& operator-=(CostMatrixInteger incrementer);

	bool operator<(CostMatrixInteger other) const;

private:
	// makes sure this CostMatrixInteger and the other CostMatrix integer are
	// available so operations can be performed on them. Throws error if not
	void CheckAvailability(CostMatrixInteger other) const;

	int value_;
	bool infinite_;
	bool available_;
};

#endif  // LITTLE_TSP_COST_MATRIX_INTEGER_H
