#include "cost_matrix.hpp"

#include <cassert>

#include <vector>
#include <algorithm>

#include "cost_matrix_integer.hpp"
#include "edge.hpp"
#include "graph/graph.hpp"
#include "matrix.hpp"

using std::for_each;
using std::min_element;
using std::vector;

using CostVector = CostMatrix::CostVector;
using CostRow = CostMatrix::CostRow;
using CostColumn = CostMatrix::CostColumn;

CostMatrix::CostMatrix(const Graph& graph, const vector<Edge>& include, 
		const vector<Edge>& exclude) :
		cost_matrix_{graph.GetNumVertices(), graph.GetNumVertices()},
		row_available_(graph.GetNumVertices(), true), 
		column_available_(graph.GetNumVertices(), true) {
	// set initial values of cells
	for (int i{0}; i < graph.GetNumVertices(); ++i) {
		for (int j{0}; j < graph.GetNumVertices(); ++j) {
			cost_matrix_(i, j) = CostMatrixInteger{graph(i, j), Edge{i, j}};
		}
	}

	// mark rows and columns that have been included as not available
	for (const Edge& e : include) {
		row_available_[e.u] = false;
		column_available_[e.v] = false;
		for (int i{0}; i < graph.GetNumVertices(); ++i) {
			cost_matrix_(e.u, i).SetUnavailable();
		}
		for (int i{0}; i < graph.GetNumVertices(); ++i) {
			cost_matrix_(i, e.v).SetUnavailable();
		}
	}

	// mark cells that have been excluded as infinite
	for (const Edge& e : exclude) { 
		CostMatrixInteger& cell_integer{cost_matrix_(e.u, e.v)};
		if (!cell_integer.IsAvailable()) { continue; }
		cost_matrix_(e.u, e.v).SetInfinite(); 
	}
}

int CostMatrix::ReduceMatrix() {
	// keep track of the amount reduced off the matrix
	int decremented{0};

	// reduce all the rows
	for (int row_num{0}; row_num < size(); ++row_num) {
		if (!row_available_[row_num]) { continue; }
		CostRow cost_row{this, row_num};
		auto min_it = min_element(cost_row.begin(), cost_row.end());
		assert(min_it != cost_row.end());
		CostMatrixInteger min{*min_it};
		assert(!min.IsInfinite());
		for_each(cost_row.begin(), cost_row.end(), 
				[&min](CostMatrixInteger& elt) { elt -= min; });
		decremented += min();
	}

	// reduce all the columns
	for (int column_num{0}; column_num < size(); ++column_num) {
		if (!column_available_[column_num]) { continue; }
		CostColumn cost_column{this, column_num};
		auto min_it = min_element(cost_column.begin(), cost_column.end());
		CostMatrixInteger min{*min_it};
		assert(!min.IsInfinite());
		for_each(cost_column.begin(), cost_column.end(), 
				[&min](CostMatrixInteger& elt) { elt -= min; });
		decremented += min();
	}

	return decremented;
}

const CostMatrixInteger& CostMatrix::operator()(int row_num, 
		int column_num) const { return cost_matrix_(row_num, column_num); }
const CostMatrixInteger& CostMatrix::operator()(const Edge& e) const { 
	return cost_matrix_(e.u, e.v); 
}

CostMatrixInteger& CostMatrix::operator()(int row_num, int column_num) {
	return cost_matrix_(row_num, column_num);
}
CostMatrixInteger& CostMatrix::operator()(const Edge& e) { 
	return cost_matrix_(e.u, e.v); 
}


CostRow CostMatrix::GetRow(int row_num) { return CostRow{this, row_num}; }

CostColumn CostMatrix::GetColumn(int column_num) { 
	return CostColumn{this, column_num};
}

const CostMatrixInteger& CostVector::operator[](int cell_num) const {
	return (*cost_matrix_)(GetRow(cell_num), GetColumn(cell_num));
}

CostMatrixInteger& CostVector::operator[](int cell_num) {
	return (*cost_matrix_)(GetRow(cell_num), GetColumn(cell_num));
}

bool CostVector::IsCellAvailable(int cell_num) const {
	return operator[](cell_num).IsAvailable();
}

CostVector::Iterator::Iterator(CostVector* cost_vector) : 
		cost_vector_{cost_vector}, traversing_cell_index_{0} { 
	// move to the next element if the first is not available
	if (!cost_vector_->IsCellAvailable(traversing_cell_index_)) {
		MoveToNextCell(); 
	}
}
	
CostMatrixInteger& CostVector::Iterator::operator*() { 
	assert(cost_vector_);
	return (*cost_vector_)[traversing_cell_index_]; 
}

CostMatrixInteger* CostVector::Iterator::operator->() {
	assert(cost_vector_);
	return &(*cost_vector_)[traversing_cell_index_];
}

CostVector::Iterator CostVector::Iterator::operator++(int) {
	int current_traversing_cell_index{traversing_cell_index_};
	MoveToNextCell();
	return Iterator{cost_vector_, current_traversing_cell_index};
}

CostVector::Iterator& CostVector::Iterator::operator++() {
	MoveToNextCell();
	return *this;
}

bool CostVector::Iterator::operator==(const CostVector::Iterator& other) const {
	return cost_vector_ == other.cost_vector_ && 
		traversing_cell_index_ == other.traversing_cell_index_;
}

bool CostVector::Iterator::operator!=(const CostVector::Iterator& other) const {
	return !(operator==(other)); 
}

void CostVector::Iterator::MoveToNextCell() {
	int vector_size{cost_vector_->cost_matrix_->size()};
	while (++traversing_cell_index_ < vector_size &&
			!cost_vector_->IsCellAvailable(traversing_cell_index_));
}

CostColumn::CostColumn(CostMatrix* cost_matrix, int column_num) : 
		CostVector{cost_matrix}, column_num_{column_num} {
	if (!IsColumnAvailable(column_num_)) {
		throw NotAvailableError{"That column is not available"};
	}
}

CostRow::CostRow(CostMatrix* cost_matrix_, int row_num) :
		CostVector{cost_matrix_}, row_num_{row_num} {
	if (!IsRowAvailable(row_num_)) {
		throw NotAvailableError{"That row is not available"};
	}
}

CostMatrixInteger CostMatrix::Iterator::operator*() { 
	return (*cost_matrix_)(row_num_, column_num_);
}

CostMatrixInteger* CostMatrix::Iterator::operator->() {
	return &(*cost_matrix_)(row_num_, column_num_);
}

CostMatrix::Iterator::Iterator(CostMatrix* cost_matrix) :
		cost_matrix_{cost_matrix}, row_num_{0}, column_num_{0} {
	// move to the next element if the first one isn't available
	if (!(*cost_matrix_)(row_num_, column_num_).IsAvailable()) { 
		MoveToNextCell();
	}
}

CostMatrix::Iterator CostMatrix::Iterator::operator++(int) {
	int current_row_num{row_num_};
	int current_column_num{column_num_};
	MoveToNextCell();
	return CostMatrix::Iterator{cost_matrix_, current_row_num, 
		current_column_num};
}

CostMatrix::Iterator& CostMatrix::Iterator::operator++() {
	MoveToNextCell();
	return *this;
}

bool CostMatrix::Iterator::operator==(const CostMatrix::Iterator& other) {
	return cost_matrix_ == other.cost_matrix_ &&
		column_num_ == other.column_num_ && row_num_ == other.row_num_;
}

bool CostMatrix::Iterator::operator!=(const CostMatrix::Iterator& other) {
	return !(operator==(other));
}

void CostMatrix::Iterator::MoveToNextCell() {
	do {
		// column_num_ wraps around
		column_num_ = (column_num_ + 1) % cost_matrix_->size();
		if (column_num_ == 0) { ++row_num_; }
	// move while the iterator is not at the end and the cell is not available
	} while (!IsAtEndOfMatrix() && 
			!(*cost_matrix_)(row_num_, column_num_).IsAvailable());
}

bool CostMatrix::Iterator::IsAtEndOfMatrix() const {
	return row_num_ == cost_matrix_->size() && column_num_ == 0;
}
