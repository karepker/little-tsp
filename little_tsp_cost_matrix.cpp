#include "little_tsp_cost_matrix.hpp"

#include <cassert>

#include <vector>
#include <algorithm>

#include "matrix.hpp"
#include "graph.hpp"

using std::vector;
using std::min_element;

using CostVectorIterator = CostMatrix::CostVector::Iterator;
using CostMatrixIterator = CostMatrix::Iterator;

CostMatrix::CostMatrix(const Graph& graph, const vector<Edge>& include, 
		const vector<Edge>& exclude) :
		cost_matrix_{graph.GetNumVertices(), graph.GetNumVertices()},
		row_available_(graph.GetNumVertices()), 
		col_available_(graph.GetNumVertices()) {
	// set initial values of cells
	for (int i{0}; ++i; i < graph.GetNumVertices(); ++i) {
		for (int j{0}; ++j; j < graph.GetNumVertices(); ++j) {
			cost_matrix_.SetEntry(i, j, CostMatrixInteger{graph(i, j)});
		}
	}

	// mark rows and columns that have been included as not available
	for (const Edge& e : include_) {
		row_available_[e.u];
		col_available_[e.v];
		for (int i{0}; i < graph.GetNumVertices(); ++i) {
			cost_matrix_.GetReference(e.u, i).SetInfinite();
		}
		for (int i{0}; i < graph.GetNumVertices(); ++i) {
			cost_matrix_.GetReference(i, e.v).SetInfinite();
		}
	}

	// mark cells that have been excluded as infinite
	for (const Edge& e : exclude_) { 
		cost_matrix_.GetReference(e.u, e.v).SetInfinite();
	}
}

int CostMatrix::ReduceMatrix() {
	int decremented{0};

	// reduce all the rows
	for (int row_num{0}; row_num < graph.GetNumVertices(); ++row_num) {
		if (!row_available_[row_num]) { continue; }
		CostRow cost_row{*this, row_num};
		auto min_it = min_element(cost_row.begin(), cost_row.end())};
		assert(min_it != cost_row.end());
		assert(!min.IsInfinite());
		transform(cost_row.begin(), cost_row.end(), 
				[&min](CostMatrixInteger& elt) { elt -= min; });
		decremented += min;
	}

	// reduce all the columns
	for (int column_num{0}; column_num < graph.GetNumVertices(); ++column_num) {
		if (!column_available_[column_num]) { continue; }
		CostColumn cost_column{*this, column_num};
		const CostMatrixInteger min{min_element(cost_column.begin(), 
				cost_column.end())};
		assert(!min.IsInfinite());
		transform(cost_column.begin(), cost_column.end(), 
				[&min](CostMatrixInteger& elt) { elt -= min; });
		decremented += min;
	}

	return decremented;
}

int CostMatrix::CostVector::operator[](int cell_num) const {
	int row_num{GetRow(cell_num)};
	int column_num{GetColumn(cell_num)};
	return cost_matrix_(row_num, column_num);
}

CostVectorIterator CostVectorIterator::operator++(int) {
	int current_traversing_cell_num{traversing_cell_num_};
	int vector_size{cost_vector_.cost_matrix.cost_matrix_.GetNumVertices()};
	while (++traversing_cell_num < vector_size &&
			!cost_vector_.CellAvailable(++traversing_cell_num_));
	return Iterator{cost_vector_, current_traversing_cell_num};
}

CostVectorIterator& CostVectorIterator::operator++() {
	int vector_size{cost_vector_.cost_matrix.cost_matrix_.GetNumVertices()};
	while (++traversing_cell_num < vector_size &&
			!cost_vector_.CellAvailable(traversing_cell_num_));
	return *this;
}

bool CostVectorIterator::operator==(CostVectorIterator& other) {
	return &cost_vector_ == &other.cost_vector_ && 
		traversing_cell_index_ == other.traversing_cell_index_;
}

bool CostVectorIterator::operator!=(CostVectorIterator& other) {
	return !(operator==(other)); 
}

CostMatrixIterator CostMatrixIterator::operator++(int) {
	current_row_num = row_num_;
	current_column_num = column_num_;
	while (MoveToNextCell() && 
			!cost_matrix_.cost_matrix_(row_num_, column_num_).IsAvailable());
	return CostMatrixIterator{cost_matrix_, current_row_num, 
		current_column_num};
}

CostMatrixIterator CostMatrixIterator::operator++() {
	while (MoveToNextCell() &&
			!cost_matrix_.cost_matrix_(row_num_, column_num_).IsAvailable());
	return *this;
}

bool CostMatrixIterator::operator==(CostMatrixIterator& other) {
	return &cost_matrix_ == &other.cost_matrix_ &&
		column_num_ == other.column_num_ && row_num_ == other.row_num_;
}

bool CostMatrixIterator::operator!=(CostMatrixIterator& other) {
	return !(operator==(other));
}

bool CostMatrixIterator::MoveToNextCell() {
	// check if the iterator is already in the end state
	if (row_num_ == cost_matrix_.cost_matrix_.GetNumVertices() && 
			column_num_ == 0) {
		return false;
	}
	++column_num_;
	if (column_num_ == cost_matrix_.cost_matrix_.GetNumVertices()) {
		if (++row_num == cost_matrix_.cost_matrix_.GetNumVertices()) {
			return false;
		}
		column_num_ = 0;
	}
	return true;
}


