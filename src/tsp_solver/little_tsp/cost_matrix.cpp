#include "tsp_solver/little_tsp/cost_matrix.hpp"

#include <cassert>

#include <algorithm>
#include <limits>
#include <unordered_map>
#include <vector>

#include "edge.hpp"
#include "graph/graph.hpp"
#include "matrix.hpp"
#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"
#include "tsp_solver/little_tsp/little_tsp_util.hpp"

using std::for_each;
using std::min_element;
using std::min;
using std::numeric_limits;
using std::unordered_map;
using std::vector;

/*
template <typename T>
using CostVector = CostMatrix::CostVector<T>;
using Row = CostMatrix::Row;
using Column = CostMatrix::Column;
*/

const int infinity{numeric_limits<int>::max()};

static vector<int> MakeVectorMapping(const vector<bool>& available);

CostMatrix::CostMatrix(const Graph& graph, const vector<Edge>& include,
		const vector<Edge>& exclude) : actual_size_{graph.GetNumVertices()} {
	vector<bool> row_available(graph.GetNumVertices(), true);
	vector<bool> column_available(graph.GetNumVertices(), true);
	int available_rows{graph.GetNumVertices()};
	// parse included edges first
	for (const Edge& e : include) {
		row_available[e.u] = false;
		column_available[e.v] = false;
		--available_rows;
	}

	// We'll just skip adding the rows/columns that are unavailable to the
	// actual Matrix data structure to save space. Call this the "condensed
	// matrix". Make a mapping of actual row/column numbers to condensed matrix 
	// row/column numbers
	row_mapping_ = MakeVectorMapping(row_available);
	column_mapping_ = MakeVectorMapping(column_available);

	row_reductions_ = vector<int>(available_rows, 0);
	column_reductions_ = vector<int>(available_rows, 0);
	cost_matrix_.SetSize(available_rows, available_rows);

	// create the condensed matrix
	for (int i{0}; i < Size(); ++i) {
		for (int j{0}; j < Size(); ++j) {
			int row_num{GetActualRowNum(i)};
			int column_num{GetActualColumnNum(j)};
			cost_matrix_(i, j) = CostMatrixInteger{graph(row_num, column_num),
				Edge{row_num, column_num}};
		}
	}

	// mark cells that have been excluded as infinite
	for (const Edge& e : exclude) {
		if (!row_available[e.u] || !column_available[e.v]) { continue; }
		// binary search the mappings to convert actual => condensed
		int condensed_row_num{
			int(lower_bound(row_mapping_.begin(), row_mapping_.end(), e.u) -
			row_mapping_.begin())};
		int condensed_column_num{int(lower_bound(
					column_mapping_.begin(), column_mapping_.end(), e.v) -
				column_mapping_.begin())};
		cost_matrix_(condensed_row_num, condensed_column_num).SetInfinite();
	}
}

int CostMatrix::ReduceMatrix() {
	assert(Size() > 1);
	// keep track of the amount reduced off the matrix
	int decremented{0};

	// find the row reductions
	for (int row_num{0}; row_num < Size(); ++row_num) {
		// find min element of row `row_num` 
		int min_elt{infinity};
		for (int column_num{0}; column_num < Size(); ++column_num)
		{ min_elt = min((*this)(row_num, column_num)(), min_elt); }
		assert(min_elt != infinity);
		row_reductions_[row_num] = min_elt;
		decremented += min_elt;
	}

	// find the column reductions
	for (int column_num{0}; column_num < Size(); ++column_num) {
		// find min element of column `column_num` 
		int min_elt{infinity};
		for (int row_num{0}; row_num < Size(); ++row_num)
		{ min_elt = min((*this)(row_num, column_num)(), min_elt); }
		assert(min_elt != infinity);
		column_reductions_[column_num] = min_elt;
		decremented += min_elt;
	}

	return decremented;
}


/*
const CostMatrixInteger& CostMatrix::operator()(int row_num,
		int column_num) const { return operator()(Edge{row_num, column_num}); }
*/
const CostMatrixInteger CostMatrix::operator()(
		int row_num, int column_num) const {
	return cost_matrix_(row_num, column_num) - row_reductions_[row_num] -
		column_reductions_[column_num];
}

CostMatrixInteger CostMatrix::operator()(int row_num, int column_num) {
	return cost_matrix_(row_num, column_num) - row_reductions_[row_num] -
		column_reductions_[column_num];
}
/*
CostMatrixInteger& CostMatrix::operator()(const Edge& e) {
	return cost_matrix_(e.u, e.v) - row_reductions_[e.u] -
		column_reductions_[e.v];
} */


/*
CostVector<Row> CostMatrix::GetRow(int row_num) {
	return CostVector<Row>{this, Row{row_num}};
}

CostVector<Column> CostMatrix::GetColumn(int column_num) {
	return CostVector<Column>{this, Column{column_num}};
}

CostMatrixInteger& CostMatrix::Iterator::operator*()
{ return (*cost_matrix_ptr_)(row_num_, column_num_); }

CostMatrixInteger* CostMatrix::Iterator::operator->()
{ return &(*cost_matrix_ptr_)(row_num_, column_num_); }

CostMatrix::Iterator CostMatrix::Iterator::operator++(int) {
	int current_row_num{row_num_};
	int current_column_num{column_num_};
	MoveToNextCell();
	return Iterator{cost_matrix_ptr_, current_row_num, current_column_num};
}

CostMatrix::Iterator& CostMatrix::Iterator::operator++() {
	MoveToNextCell();
	return *this;
}

bool CostMatrix::Iterator::operator==(const CostMatrix::Iterator& other) {
	return cost_matrix_ptr_ == other.cost_matrix_ptr_ &&
		column_num_ == other.column_num_ && row_num_ == other.row_num_;
}

bool CostMatrix::Iterator::operator!=(const CostMatrix::Iterator& other)
{ return !(operator==(other)); }

void CostMatrix::Iterator::MoveToNextCell() {
	// check if at end of matrix
	if (row_num_ == cost_matrix_ptr_->GetNumRows() && column_num_ == 0)
	{ return; }

	// update row and column number, so column_num_ wraps around
	column_num_ = (column_num_ + 1) % cost_matrix_ptr_->GetNumColumns();
	if (column_num_ == 0) { ++row_num_; }
}

*/

vector<int> MakeVectorMapping(const vector<bool>& available) {
	vector<int> mapping;

	// make mapping "condensed matrix row/column" => "actual row/column"
	for (int cell_num{0}; cell_num < int(available.size()); ++cell_num) {
		if (!available[cell_num]) { continue; }
		mapping.push_back(cell_num);
	}
	return mapping;
}
