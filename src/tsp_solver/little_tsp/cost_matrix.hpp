#ifndef LITTLE_TSP_COST_MATRIX_H
#define LITTLE_TSP_COST_MATRIX_H

#include <unordered_map>
#include <vector>

#include "matrix.hpp"

class Graph;
class CostMatrixInteger;
struct Edge;

// information about the useable matrix
// temporary structure that is used to help build a TreeNode
class CostMatrix {
public:
	CostMatrix(const Graph& graph, const std::vector<Edge>& include, 
			const std::vector<Edge>& exclude);

	int ReduceMatrix();

	const CostMatrixInteger& operator()(int row_num, int column_num) const;
	const CostMatrixInteger& operator()(const Edge& e) const;
	CostMatrixInteger& operator()(int row_num, int column_num);
	CostMatrixInteger& operator()(const Edge& e);

	bool IsRowAvailable(int row_num) const
	{ return row_mapping_.count(row_num) == 1; }
	bool IsColumnAvailable(int column_num) const
	{ return column_mapping_.count(column_num) == 1; }

	int Size() const { return cost_matrix_.GetNumRows(); }

	class CostVector;
	class CostRow; 
	class CostColumn;
	// these classes need to be able to share data with each other because they
	// are very intimately related
	friend class CostVector;

	CostRow GetRow(int row_num);
	CostColumn GetColumn(int column_num);

	// encapsulation of cost information about either a single row or a column
	// defines iterator which allows easier traversal and more use of STL
	class CostVector {
	public:
		CostVector() : cost_matrix_ptr_{nullptr} {}
		explicit CostVector(Matrix<CostMatrixInteger>* cost_matrix_ptr) : 
			cost_matrix_ptr_{cost_matrix_ptr} {}

		const CostMatrixInteger& operator[](int cell_num) const;
		CostMatrixInteger& operator[](int cell_num);
		// GetNumRows and GetNumColumns are equivalent for square cost matrix
		int Size() const { return cost_matrix_ptr_->GetNumRows(); }

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

	protected:
		// define interface for derived classes because CostVector doesn't know
		// if it's a row or column
		virtual int GetRow(int cell_num) const = 0;
		virtual int GetColumn(int cell_num) const = 0;

	private:
		Matrix<CostMatrixInteger>* cost_matrix_ptr_;
	};

	class CostColumn : public CostVector {
	public:
		CostColumn(Matrix<CostMatrixInteger>* cost_matrix_ptr, int column_num);

	private:
		int GetRow(int cell_num) const override { return cell_num; }
		int GetColumn(int) const override { return column_num_; }

		int column_num_;
	};

	class CostRow : public CostVector {
	public:
		CostRow(Matrix<CostMatrixInteger>* cost_matrix_ptr, int row_num);

	private:
		int GetColumn(int cell_num) const override { return cell_num; }
		int GetRow(int) const override { return row_num_; }

		int row_num_;
	};

	// iterator for iterating over all cells in the cost matrix
	class Iterator {
	public:
		Iterator() : cost_matrix_ptr_{nullptr}, row_num_{0}, column_num_{0} {}
		explicit Iterator(Matrix<CostMatrixInteger>* cost_matrix_ptr) : 
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
		Iterator(Matrix<CostMatrixInteger>* cost_matrix_ptr, int row_num, 
				int column_num) : cost_matrix_ptr_{cost_matrix_ptr}, 
			row_num_{row_num}, column_num_{column_num} {}

		void MoveToNextCell();

		Matrix<CostMatrixInteger>* cost_matrix_ptr_;
		int row_num_; 
		int column_num_;
	};

	Iterator begin() { return Iterator{&cost_matrix_}; }
	Iterator end() { return Iterator{&cost_matrix_, Size(), 0}; }
	
private:
	int GetCondensedRowNum(int row_num) const;
	int GetCondensedColumnNum(int row_num) const;

	Matrix<CostMatrixInteger> cost_matrix_;
	std::unordered_map<int, int> row_mapping_;
	std::unordered_map<int, int> column_mapping_;
};


#endif  // LITTLE_TSP_COST_MATRIX_H
