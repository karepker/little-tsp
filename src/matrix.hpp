#ifndef MATRIX_H
#define MATRIX_H

#include <cassert>

#include <vector>

template <class T>
class Matrix {
public:
	Matrix() : rows_{0} {}
	Matrix(int rows, int cols) : rows_{rows}, entries_(rows * cols) {}
	Matrix(int rows, int cols, T initial_value) : rows_{rows}, 
		entries_(rows * cols, initial_value) {}
	Matrix(int rows, std::vector<T> entries) : rows_{rows}, entries_{entries} {
		// size of entries must be rows_ * <integer value>
		assert(entries_.size() % rows == 0);
	}

	void SetSize(int rows, int cols) { 
		rows_ = rows;
		entries_.resize(rows * cols);
	}

	// operators for getting costs
	T& operator()(int row, int col) { return entries_[row * rows_ + col]; }
	const T& operator()(int row, int col) const { 
		return entries_[row * rows_ + col]; 
	}

	int GetNumRows() const { return rows_; }
	int GetNumColumns() const { return entries_.size() / rows_; }

private:
	int rows_;
	std::vector<T> entries_;
};

#endif // MATRIX_H
