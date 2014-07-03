#ifndef TSP_SOLVER_LITTLE_TSP_COST_MATRIX_H
#define TSP_SOLVER_LITTLE_TSP_COST_MATRIX_H

#include <unordered_map>
#include <utility>
#include <vector>

#include "matrix.hpp"
#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"

class Graph;
struct Edge;
struct CostMatrixZero;

// information about the useable matrix
// temporary structure that is used to help build a TreeNode
class CostMatrix {
public:
	CostMatrix(const Graph& graph, const std::vector<Edge>& include,
			const std::vector<Edge>& exclude);

	int ReduceMatrix();

	const CostMatrixInteger operator()(int row_num, int column_num) const;
	//const CostMatrixInteger& operator()(const Edge& e) const;
	CostMatrixInteger operator()(int row_num, int column_num);
	//CostMatrixInteger& operator()(const Edge& e);

	int Size() const { return cost_matrix_.GetNumRows(); }
	int GetActualSize() const { return actual_size_; }

	/*
	template <typename T>
	class CostVector;
	// these classes need to be able to share data with each other because they
	// are very intimately related
	class Column;
	class Row;
	friend class CostVector<Column>;
	friend class CostVector<Row>;

	CostVector<Row> GetRow(int row_num);
	CostVector<Column> GetColumn(int column_num);

	// encapsulation of cost information about either a single row or a column
	// defines iterator which allows easier traversal and more use of STL
	template <typename T>
	class CostVector {
	public:
		CostVector(CostMatrix* cost_matrix_ptr,
				const T orientation) : cost_matrix_ptr_{cost_matrix_ptr},
			orientation_{orientation} {}

		const CostMatrixInteger& operator[](int cell_num) const;
		CostMatrixInteger& operator[](int cell_num);
		// GetNumRows and GetNumColumns are equivalent for square cost matrix
		int Size() const { return cost_matrix_ptr_->Size(); }

		// we need access to the protected interface in cost vector and the
		// subscripting operation
		class Iterator;
		friend class Iterator;

		class Iterator {
		public:
			explicit Iterator(CostVector* cost_vector_ptr) :
				cost_vector_ptr_{cost_vector_ptr}, traversing_cell_index_{0} {}
			Iterator() : cost_vector_ptr_{nullptr}, traversing_cell_index_{0} {}

			CostMatrixInteger& operator*();
			CostMatrixInteger* operator->();

			Iterator operator++(int);
			Iterator& operator++();

			bool operator==(const Iterator& other) const;
			bool operator!=(const Iterator& other) const;

			friend class CostVector;

		private:
			Iterator(CostVector* cost_vector_ptr, int cell_num) :
				cost_vector_ptr_{cost_vector_ptr},
				traversing_cell_index_{cell_num} {}

			CostVector* cost_vector_ptr_;
			int traversing_cell_index_;  // changes as iterator moves
		};

		Iterator begin() { return Iterator{this}; }
		Iterator end() { return Iterator{this, Size()}; }

	private:
		CostMatrix* cost_matrix_ptr_;
		const T orientation_;  // either Row or Column
	};

	class Column {
	public:
		explicit Column(int column_num) : column_num_{column_num} {}

		int GetRow(int cell_num) const { return cell_num; }
		int GetColumn(int) const { return column_num_; }

	private:
		int column_num_;
	};

	class Row {
	public:
		explicit Row(int row_num) : row_num_{row_num} {}

		int GetColumn(int cell_num) const { return cell_num; }
		int GetRow(int) const { return row_num_; }

	private:
		int row_num_;
	};

	// iterator for iterating over all cells in the cost matrix
	class Iterator {
	public:
		Iterator() : cost_matrix_ptr_{nullptr}, row_num_{0}, column_num_{0} {}
		explicit Iterator(CostMatrix* cost_matrix_ptr) :
			cost_matrix_ptr_{cost_matrix_ptr}, row_num_{0}, column_num_{0} {}

		CostMatrixInteger& operator*();

		CostMatrixInteger* operator->();

		Iterator operator++(int);
		Iterator& operator++();

		bool operator==(const Iterator& other);
		bool operator!=(const Iterator& other);

		// allows CostMatrix to to create an end() Iterator
		friend class CostMatrix;

	private:
		Iterator(CostMatrix* cost_matrix_ptr, int row_num, int column_num) :
			cost_matrix_ptr_{cost_matrix_ptr}, row_num_{row_num},
			column_num_{column_num} {}

		void MoveToNextCell();

		CostMatrix* cost_matrix_ptr_;
		int row_num_;
		int column_num_;
	};

	Iterator begin() { return Iterator{this}; }
	Iterator end() { return Iterator{this, Size(), 0}; } */

private:
	int GetActualRowNum(int row_num) const { return row_mapping_[row_num]; }
	int GetActualColumnNum(int column_num) const
	{ return column_mapping_[column_num]; }

	int actual_size_;
	Matrix<CostMatrixInteger> cost_matrix_;
	// map actual cell => condensed cell
	std::vector<int> row_mapping_;
	std::vector<int> column_mapping_;
	std::vector<int> row_reductions_;
	std::vector<int> column_reductions_;
};

/*
template <typename T>
const CostMatrixInteger& CostMatrix::CostVector<T>::operator[](
		int cell_num) const {
	return (*cost_matrix_ptr_)(orientation_.GetRow(cell_num),
		orientation_.GetColumn(cell_num));
}

template <typename T>
CostMatrixInteger& CostMatrix::CostVector<T>::operator[](int cell_num) {
	return (*cost_matrix_ptr_)(orientation_.GetRow(cell_num),
			orientation_.GetColumn(cell_num));
}

template <typename T>
CostMatrixInteger& CostMatrix::CostVector<T>::Iterator::operator*() {
	assert(cost_vector_ptr_);
	return (*cost_vector_ptr_)[traversing_cell_index_];
}

template <typename T>
CostMatrixInteger* CostMatrix::CostVector<T>::Iterator::operator->() {
	assert(cost_vector_ptr_);
	return &(*cost_vector_ptr_)[traversing_cell_index_];
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

*/

#endif  // TSP_SOLVER_LITTLE_TSP_COST_MATRIX_H
