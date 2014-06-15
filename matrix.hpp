#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

template <class T>
class Matrix {
public:
	Matrix() : rows_{0}, entries_{0} {}
	Matrix(int rows, int cols) : rows_{rows}, entries_(rows * cols) {}
	Matrix(int rows, int cols, T initial_value) : rows_{rows}, 
		entries_(rows * cols, initial_value) {}

	void SetEntries(int rows, int cols) { 
		rows_ = rows;
		entries_.resize(rows * cols);
	}

	// operators for getting costs
	T operator()(int row, int col) const
	{ return entries_[row * rows_ + col]; }

	T& GetReference(int row, int col)
	{ return entries_[row * rows_ + col]; }

	// set the entry
	void SetEntry(int row, int col, T value)
	{ entries_[row * rows_ + col] = value; }

	int size() { return rows_; }

private:
	int rows_;
	std::vector<T> entries_;
};

#endif // MATRIX_H
