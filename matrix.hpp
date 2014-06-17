#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <utility>

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
	T& operator()(int row, int col) { return entries_[row * rows_ + col]; }
	const T& operator()(int row, int col) const { 
		return entries_[row * rows_ + col]; 
	}

	std::pair<int, int> size() const {
		return std::make_pair(rows_, entries_.size() / rows_); 
	}

private:
	int rows_;
	std::vector<T> entries_;
};

#endif // MATRIX_H
