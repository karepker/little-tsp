#ifndef TSP_SOLVER_LITTLE_TSP_COST_MATRIX_H
#define TSP_SOLVER_LITTLE_TSP_COST_MATRIX_H

#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "graph/edge_cost.hpp"
#include "matrix.hpp"

struct Edge;
class Graph;

// Pseudo-container that serves as a view for the graph handling computation and
// iteration so TreeNode doesn't have to. "Pseudo-container" because it does not
// actually store elements, and computes them on the fly using the elements in
// the graph.
//
// CostMatrix keeps track of a condensed index system built on construction,
// which simplifies iteration by guaranteeing condensed indices map only to
// available rows and columns.
class CostMatrix {
public:
	CostMatrix(const Graph& graph, const std::vector<Edge>& include,
			const Matrix<int>& exclude);

	// Reduces the matrix by subtracting the smallest value in each row and
	// column such that for all rows and columns there exists an element == 0.
	// Return the total value that was deducted from the matrix.
	int ReduceMatrix();

	// Calculate the value at the given row and column based on the reduction
	// and whether the cell has been marked infinite.
	EdgeCost operator()(int row_num, int column_num) const;

	// Get the full size of the cost matrix (= number of vertices in the graph).
	int GetActualSize() const;
	// Get the size of the matrix using condensed indexing.
	int GetCondensedSize() const { return condensed_size_; }

	// Map from condensed row => actual row number.
	int GetActualRowNum(int row_num) const { return row_mapping_[row_num]; }
	// Map from condensed column => actual column number.
	int GetActualColumnNum(int column_num) const
	{ return column_mapping_[column_num]; }

	class Row;
	class Column;

	// Encapsulation of cost information about either a single row or a column.
	// Uses templates instead of OOP for increased speed.
	template <typename T>
	class CostVector {
	public:
		static_assert(std::is_same<T, Row>::value ||
				std::is_same<T, Column>::value,
				"Must construct CostVector with either a Row or a Column");

		CostVector(const CostMatrix* cost_matrix_ptr,
				const T orientation) : cost_matrix_ptr_{cost_matrix_ptr},
			orientation_{orientation} {}

		// Gets the EdgeCost from the appropriate location in the cost matrix.
		EdgeCost operator[](int cell_num) const;
		// GetNumRows and GetNumColumns are equivalent for square cost matrix
		int Size() const { return cost_matrix_ptr_->GetCondensedSize(); }

		// Pseudo-iterator allows easier iteration over vector and more use of
		// STL. Note: "pseudo" because it does not implement operator->()
		// because CostMatrix is a pseudo-container and computes elements on
		// the fly.
		class Iterator {
		public:
			explicit Iterator(const CostVector* cost_vector_ptr) :
				cost_vector_ptr_{cost_vector_ptr}, traversing_cell_index_{0} {}
			Iterator() : cost_vector_ptr_{nullptr}, traversing_cell_index_{0} {}

			EdgeCost operator*() const;

			Iterator operator++(int);
			Iterator& operator++();

			bool operator==(const Iterator& other) const;
			bool operator!=(const Iterator& other) const;

			friend class CostVector;

		private:
			Iterator(const CostVector* cost_vector_ptr, int cell_num) :
				cost_vector_ptr_{cost_vector_ptr},
				traversing_cell_index_{cell_num} {}

			const CostVector* cost_vector_ptr_;
			int traversing_cell_index_;  // changes as iterator moves
		};

		Iterator begin() const { return Iterator{this}; }
		Iterator end() const { return Iterator{this, Size()}; }

	private:
		const CostMatrix* cost_matrix_ptr_;
		const T orientation_;  // either Row or Column
	};

	class Column {
	public:
		explicit Column(int column_num) : column_num_{column_num} {}

		int GetRow(int cell_num, const CostMatrix* cost_matrix_ptr) const
		{ return cost_matrix_ptr->GetActualRowNum(cell_num); }
		int GetColumn(int, const CostMatrix*) const { return column_num_; }

	private:
		int column_num_;
	};

	class Row {
	public:
		explicit Row(int row_num) : row_num_{row_num} {}

		int GetColumn(int cell_num, const CostMatrix* cost_matrix_ptr) const
		{ return cost_matrix_ptr->GetActualColumnNum(cell_num); }
		int GetRow(int, const CostMatrix*) const { return row_num_; }

	private:
		int row_num_;
	};

	// Helper functions that get cost vectors representing the given row or
	// column.
	CostVector<Row> GetRow(int row_num);
	CostVector<Column> GetColumn(int column_num);

	// Pseudo-iterator allows easier iteration over entire matrix and more use
	// of STL. Note: "pseudo" because it does not implement operator->() because 
	// CostMatrix is a pseudo-container and computes elements on the fly.
	class Iterator {
	public:
		Iterator() : cost_matrix_ptr_{nullptr}, row_num_{0}, column_num_{0} {}
		explicit Iterator(const CostMatrix* cost_matrix_ptr) :
			cost_matrix_ptr_{cost_matrix_ptr}, row_num_{0}, column_num_{0} {}

		EdgeCost operator*();
		//EdgeCost* operator->();

		Iterator operator++(int);
		Iterator& operator++();

		bool operator==(const Iterator& other);
		bool operator!=(const Iterator& other);

		// allows CostMatrix to to create an end() Iterator
		friend class CostMatrix;

	private:
		Iterator(const CostMatrix* cost_matrix_ptr, int row_num,
				int column_num) : cost_matrix_ptr_{cost_matrix_ptr},
			row_num_{row_num}, column_num_{column_num} {}

		void MoveToNextCell();

		const CostMatrix* cost_matrix_ptr_;
		int row_num_;
		int column_num_;
	};

	Iterator begin() const { return Iterator{this}; }
	Iterator end() const { return Iterator{this, GetCondensedSize(), 0}; }

private:
	// allow CostVector to call GetCondensedSize()
	friend class CostVector<Row>;
	friend class CostVector<Column>;

		const Graph& graph_;
	const Matrix<int>& infinite_;  // reference to TreeNode's exclude_ matrix
	int condensed_size_;

	// map actual cell => condensed cell
	std::vector<int> row_mapping_;
	std::vector<int> column_mapping_;

	// hold the reductions for rows and columns
	std::vector<int> row_reductions_;
	std::vector<int> column_reductions_;
};

template <typename T>
EdgeCost CostMatrix::CostVector<T>::operator[](int cell_num) const {
	return (*cost_matrix_ptr_)(orientation_.GetRow(cell_num, cost_matrix_ptr_),
			orientation_.GetColumn(cell_num, cost_matrix_ptr_));
}

template <typename T>
EdgeCost CostMatrix::CostVector<T>::Iterator::operator*() const {
	assert(cost_vector_ptr_);
	return (*cost_vector_ptr_)[traversing_cell_index_];
}

template <typename T>
typename CostMatrix::CostVector<T>::Iterator
CostMatrix::CostVector<T>::Iterator::operator++(int) {
	int current_traversing_cell_index{traversing_cell_index_};
	if (traversing_cell_index_ < cost_vector_ptr_->Size())
	{ ++traversing_cell_index_; }
	return Iterator{cost_vector_ptr_, current_traversing_cell_index};
}

template <typename T>
typename CostMatrix::CostVector<T>::Iterator&
CostMatrix::CostVector<T>::Iterator::operator++() {
	if (traversing_cell_index_ < cost_vector_ptr_->Size())
	{ ++traversing_cell_index_; }
	return *this;
}

template <typename T>
bool CostMatrix::CostVector<T>::Iterator::operator==(
		const CostVector::Iterator& other) const {
	return cost_vector_ptr_ == other.cost_vector_ptr_ &&
		traversing_cell_index_ == other.traversing_cell_index_;
}

template <typename T>
bool CostMatrix::CostVector<T>::Iterator::operator!=(
		const CostVector::Iterator& other) const
{ return !(operator==(other)); }


#endif  // TSP_SOLVER_LITTLE_TSP_COST_MATRIX_H
