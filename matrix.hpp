#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

template <class T>
class Matrix {
public:
	Matrix() {}
	Matrix(int num) : entries(num) {}

	// operators for getting costs
	T operator()(int row, int col)
	{ return entries[row * this->size + col]; }

	// set the entry
	void setEntry(int row, int col, T value)
	{ entries[row * this->size + col] = value; }

private:
	std::vector<T> entries_;
};

#endif // MATRIX_H
