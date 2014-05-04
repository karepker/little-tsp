#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

template <class T>
struct Matrix
{
	Matrix() {}
	Matrix(int num) : entries{num} {}

	// operators for getting costs
	T operator()(int row, int col)
	{ return this->entries[row * this->size + col]; }

	// set the entry
	void setEntry(int row, int col, T value)
	{ this->entries[row * this->size + col] = value; }

	std::vector<T> entries;
	int size;
};

#endif // MATRIX_H
