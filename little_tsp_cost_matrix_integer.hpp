#ifndef LITTLE_TSP_COST_MATRIX_INTEGER_H
#define LITTLE_TSP_COST_MATRIX_INTEGER_H

/** 
 * class used to store integers and their states in a cost matrix in Little's
 * TSP algorithm. An integer's availability corresponds to whether or not it has
 * been "crossed out" in the cost matrix. An integer's infinite status
 * corresponds to whether or not it should be regarded as infinite
 */
class CostMatrixInteger {
public:
	CostMatrixInteger(int value);
	CostMatrixInteger(int value, bool infinite, bool available);

	// A whole bunch of overloaded operators. These try and follow the rules of
	// math when an integer is infinite, and throw an error if its not available
	CostMatrixInteger operator-(CostMatrixInteger a, CostMatrixInteger b);
	CostMatrixInteger operator+(CostMatrixInteger a, CostMatrixInteger b);
	CostMatrixInteger operator*(CostMatrixInteger a, CostMatrixInteger b);
	CostMatrixInteger operator/(CostMatrixInteger a, CostMatrixInteger b);

	CostMatrixInteger& operator+=(CostMatrixInteger incrementer);
	CostMatrixInteger& operator-=(CostMatrixInteger incrementer);

private:
	int value_;
	bool infinite_;
	bool available_;
};

#endif  // LITTLE_TSP_COST_MATRIX_INTEGER_H
