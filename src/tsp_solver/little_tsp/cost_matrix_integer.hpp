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
	// default construction is non-available
	CostMatrixInteger();
	// creates an available, finite integer with the given value
	explicit CostMatrixInteger(int value);
	// creates an integer with value -1, typically should be used only when
	// integer is infinite or not available
	CostMatrixInteger(bool infinite, bool available);
	// allows control of all three parameters
	CostMatrixInteger(int value, bool infinite, bool available);

	void SetInfinite();
	void SetUnavailable() { available_ = false; }

	// returns the value, throws an error if not available
	int operator()() const;
	bool IsAvailable() const { return available_; }
	bool IsInfinite() const;

	// A whole bunch of overloaded operators. These try and follow the rules of
	// math when an integer is infinite, and throw an error if its not available
	CostMatrixInteger operator+(CostMatrixInteger other) const;
	CostMatrixInteger operator-(CostMatrixInteger other) const;

	CostMatrixInteger& operator+=(CostMatrixInteger incrementer);
	CostMatrixInteger& operator-=(CostMatrixInteger decrementer);

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
