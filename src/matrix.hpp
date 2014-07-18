#ifndef MATRIX_H
#define MATRIX_H

#include <cassert>

#include <type_traits>
#include <vector>

template <typename T>
class Matrix {
public:
	static_assert(!std::is_same<T, bool>::value, "Cannot use boolean with this "
			"matrix class.");

	Matrix() : rows_{0} {}
	Matrix(int square_size) : rows_{square_size},
		entries_(square_size * square_size) {}
	Matrix(int rows, int cols) : rows_{rows}, entries_(rows * cols) {}
	Matrix(int rows, int cols, T initial_value) : rows_{rows},
		entries_(rows * cols, initial_value) {}

	// size of entries must be rows_ * <integer value>
	Matrix(int rows, std::vector<T> entries) : rows_{rows}, entries_{entries}
	{ assert(entries_.size() % rows == 0); }

	void SetSize(int rows, int cols) {
		rows_ = rows;
		entries_.resize(rows * cols);
	}
	void SetSize(int square_size) {
		rows_ = square_size;
		entries_.resize(square_size * square_size);
	}

	// operators for getting costs
	T& operator()(int row, int col) { return entries_[row * rows_ + col]; }
	const T& operator()(int row, int col) const
	{ return entries_[row * rows_ + col]; }

	int GetNumRows() const { return rows_; }
	int GetNumColumns() const { return entries_.size() / rows_; }

private:
	int rows_;
	std::vector<T> entries_;
};

#endif // MATRIX_H
