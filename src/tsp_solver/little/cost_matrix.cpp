#include "tsp_solver/little/cost_matrix.hpp"

#include <cassert>

#include <algorithm>
#include <limits>
#include <unordered_map>
#include <vector>

#include "graph/edge.hpp"
#include "graph/edge_cost.hpp"
#include "graph/graph.hpp"
#include "matrix.hpp"
#include "util.hpp"

using std::for_each;
using std::min_element;
using std::min;
using std::numeric_limits;
using std::unordered_map;
using std::vector;

template <typename T>
using CostVector = CostMatrix::CostVector<T>;
using Row = CostMatrix::Row;
using Column = CostMatrix::Column;

const int infinity{numeric_limits<int>::max()};

static vector<int> MakeVectorMapping(const vector<bool>& available);

CostMatrix::CostMatrix(const Graph& graph, const vector<Edge>& include,
		const Matrix<int>& exclude) : graph_{graph}, infinite_{exclude} {
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
	condensed_size_ = available_rows;

	row_reductions_ = vector<int>(GetActualSize(), 0);
	column_reductions_ = vector<int>(GetActualSize(), 0);
}

int CostMatrix::ReduceMatrix() {
	// keep track of the amount reduced off the matrix
	int decremented{0};

	// find the row reductions
	for (int row_num{0}; row_num < GetCondensedSize(); ++row_num) {
		CostVector<Row> row{GetRow(GetActualRowNum(row_num))};
		// find min element of row `row_num`
		auto min_elt_it = min_element(row.begin(), row.end());
		assert(min_elt_it != row.end());
		int min_elt{(*min_elt_it)()};
		// abandon ship if the reduction is infinite
		if (min_elt == infinity) { return infinity; }
		row_reductions_[GetActualRowNum(row_num)] = min_elt;
		decremented += min_elt;
	}

	// find the column reductions
	for (int column_num{0}; column_num < GetCondensedSize(); ++column_num) {
		CostVector<Column> column{GetColumn(GetActualColumnNum(column_num))};
		// find min element of column `column_num`
		auto min_elt_it = min_element(column.begin(), column.end());
		assert(min_elt_it != column.end());
		int min_elt{(*min_elt_it)()};
		// abandon ship if the reduction is infinite
		if (min_elt == infinity) { return infinity; }
		column_reductions_[GetActualColumnNum(column_num)] = min_elt;
		decremented += min_elt;
	}

	return decremented;
}

int CostMatrix::GetActualSize() const { return graph_.GetNumVertices(); }

EdgeCost CostMatrix::operator()(int row_num, int column_num) const {
	if (infinite_(row_num, column_num))
	{ return EdgeCost::Infinite(Edge{row_num, column_num}); }
	return graph_(row_num, column_num) - row_reductions_[row_num] -
		column_reductions_[column_num];
}

CostVector<Row> CostMatrix::GetRow(int row_num) {
	return CostVector<Row>{this, Row{row_num}};
}

CostVector<Column> CostMatrix::GetColumn(int column_num) {
	return CostVector<Column>{this, Column{column_num}};
}

EdgeCost CostMatrix::Iterator::operator*() {
	return (*cost_matrix_ptr_)(
			cost_matrix_ptr_->GetActualRowNum(row_num_),
			cost_matrix_ptr_->GetActualColumnNum(column_num_));
}

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
	if (row_num_ == cost_matrix_ptr_->GetCondensedSize() && column_num_ == 0)
	{ return; }

	// update row and column number, so column_num_ wraps around
	column_num_ = (column_num_ + 1) % cost_matrix_ptr_->GetCondensedSize();
	if (column_num_ == 0) { ++row_num_; }
}

// Map condensed index => actual index such that condensed indices always point
// to rows and columns that are available. This is useful for iteration in
// CostVector.
vector<int> MakeVectorMapping(const vector<bool>& available) {
	vector<int> mapping;

	// make mapping "condensed matrix row/column" => "actual row/column"
	for (int cell_num{0}; cell_num < int(available.size()); ++cell_num) {
		if (!available[cell_num]) { continue; }
		mapping.push_back(cell_num);
	}
	return mapping;
}
