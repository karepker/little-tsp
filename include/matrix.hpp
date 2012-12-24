#ifndef __MATRIX__
#define __MATRIX__

// INCLUDES
// STL
#include <vector>

template <class T>
struct Matrix
{
	std::vector<T> entries;
	unsigned int size;

	// operators for getting costs
	const T operator()(unsigned int row, unsigned int col) const
	{ return this->entries[row * this->size + col]; };
	T operator()(unsigned int row, unsigned int col)
	{ return this->entries[row * this->size + col]; };

	// set the entry
	void setEntry(unsigned int row, unsigned int col, T value)
	{ this->entries[row * this->size + col] = value; };

	Matrix(unsigned int num) : entries(num) {};
	Matrix() {};

};



#endif // __MATRIX__
