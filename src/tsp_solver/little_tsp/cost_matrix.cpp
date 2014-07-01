#include "tsp_solver/little_tsp/cost_matrix.hpp"

#include <cassert>

#include <algorithm>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "edge.hpp"
#include "graph/graph.hpp"
#include "matrix.hpp"
#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"

using std::for_each;
using std::min_element;
using std::pair;
using std::make_pair;
using std::max;
using std::numeric_limits;
using std::unordered_map;
using std::vector;

template <typename T>
using CostVector = CostMatrix::CostVector<T>;
using Row = CostMatrix::Row;
using Column = CostMatrix::Column;

using cmi_pair_t = std::pair<CostMatrixInteger, CostMatrixInteger>;
using two_smallest_t = std::vector<cmi_pair_t>;

const int infinity{numeric_limits<int>::max()};

static vector<int> MakeVectorMapping(const vector<bool>& available);
static void UpdateTwoSmallest(const CostMatrixInteger& current,
		pair<CostMatrixInteger, CostMatrixInteger>& two_smallest);
static CostMatrixInteger GetPenalty(const Edge& edge,
		const cmi_pair_t& penalties);
static vector<CostMatrixZero> FindBaseCaseZerosAndPenalties(
		const vector<Edge>& zero_edges,
		const two_smallest_t& two_smallest_row,
		const two_smallest_t& two_smallest_column);

CostMatrix::CostMatrix(const Graph& graph, const vector<Edge>& include,
		const vector<Edge>& exclude) {
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

	cost_matrix_.SetSize(available_rows, available_rows);

	// create the condensed matrix
	for (int i{0}; i < graph.GetNumVertices(); ++i) {
		if (!row_available[i]) { continue; }
		for (int j{0}; j < graph.GetNumVertices(); ++j) {
			if (!column_available[j]) { continue; }
			(*this)(i, j) = CostMatrixInteger{graph(i, j), Edge{i, j}};
		}
	}

	// mark cells that have been excluded as infinite
	for (const Edge& e : exclude) {
		if (!IsRowAvailable(e.u) || !IsColumnAvailable(e.v)) { continue; }
		(*this)(e.u, e.v).SetInfinite();
	}
}

int CostMatrix::ReduceMatrix() {
	// keep track of the amount reduced off the matrix
	int decremented{0};

	// find the row reductions
	for (int row_num{0}; row_num < Size(); ++row_num) {
		CostVector<Row> cost_row{&cost_matrix_, Row{row_num}};
		auto min_it = min_element(cost_row.begin(), cost_row.end());
		assert(min_it != cost_row.end());
		CostMatrixInteger min{*min_it};
		assert(!min.IsInfinite());
		for_each(cost_row.begin(), cost_row.end(),
				[&min](CostMatrixInteger& elt) { elt -= min; });
		decremented += min();
	}

	// find the column reductions
	for (int column_num{0}; column_num < Size(); ++column_num) {
		CostVector<Column> cost_column{&cost_matrix_,
			Column{column_num}};
		auto min_it = min_element(cost_column.begin(), cost_column.end());
		CostMatrixInteger min{*min_it};
		assert(!min.IsInfinite());
		for_each(cost_column.begin(), cost_column.end(),
				[&min](CostMatrixInteger& elt) { elt -= min; });
		decremented += min();
	}

	return decremented;
}

vector<CostMatrixZero> CostMatrix::FindZerosAndPenalties() const {
	// hold the two smallest elements in each row and column
	cmi_pair_t infinite_cmis{make_pair(CostMatrixInteger::Infinite(),
		CostMatrixInteger::Infinite())};
	two_smallest_t two_smallest_row(Size(), infinite_cmis);
	two_smallest_t two_smallest_column(Size(), infinite_cmis);

	// hold the zeros
	vector<Edge> zero_edges;

	// find zeros and store two smallest elements
	for (int i{0}; i < Size(); ++i) {
		for (int j{0}; j < Size(); ++j) {
			const CostMatrixInteger& current{cost_matrix_(i, j)};
			// find zeros
			if (current() == 0) { zero_edges.push_back(current.GetEdge()); }
			// look for two smallest elements
			UpdateTwoSmallest(current, two_smallest_row[i]);
			UpdateTwoSmallest(current, two_smallest_column[j]);
		}
	}

	// 3 cases
	// 1. base case: 2 edges left to add. 
	// Logic is sufficiently different that it belongs in its own function 
	if (Size() == 2) {
		return FindBaseCaseZerosAndPenalties(
				zero_edges, two_smallest_row, two_smallest_column);
	}

	// hold a dummy value for max now that will be replaced on first iteration
	vector<CostMatrixZero> cost_matrix_zeros;
	cost_matrix_zeros.push_back(CostMatrixZero{Edge{-1, -1}, -1});

	for (const Edge& edge : zero_edges) {
		// get the row and column penalties
		CostMatrixInteger row_penalty{GetPenalty(edge,
				two_smallest_row[GetCondensedRowNum(edge.u)])};
		CostMatrixInteger column_penalty{GetPenalty(edge,
				two_smallest_column[GetCondensedColumnNum(edge.v)])};

		// 2. case when excluding the node creates a disconnected graph
		// we must choose this edge and cannot branch, so return vector with
		// single element that is this zero
		if (column_penalty.IsInfinite() != row_penalty.IsInfinite())
		{ return { CostMatrixZero{edge, infinity} }; }

		// 3. normal case, there is both an include and exclude branch
		// keep only the structure with the maximum penalty
		CostMatrixZero current_zero{edge, row_penalty() + column_penalty()};
		cost_matrix_zeros[0] = max(cost_matrix_zeros[0], current_zero);
	}
	return cost_matrix_zeros;
}

vector<CostMatrixZero> FindBaseCaseZerosAndPenalties(
		const vector<Edge>& zero_edges,
		const two_smallest_t& two_smallest_row,
		const two_smallest_t& two_smallest_column) {

	vector<CostMatrixZero> cost_matrix_zeros;
	for (const Edge& edge : zero_edges) {
		// get the row and column penalties
		CostMatrixInteger row_penalty{
			GetPenalty(edge, two_smallest_row[edge.u])};
		CostMatrixInteger column_penalty{
			GetPenalty(edge, two_smallest_column[edge.v])};

		// We need to know if penalty is zero or infinite so we can choose the 
		// two edges with the highest penalties
		if (column_penalty.IsInfinite() || row_penalty.IsInfinite())
		{ cost_matrix_zeros.push_back(CostMatrixZero{edge, infinity}); }
		else {
			assert(column_penalty() == 0 && row_penalty() == 0);
			cost_matrix_zeros.push_back(CostMatrixZero{edge, 0});
		}
	}
	return cost_matrix_zeros;
}

const CostMatrixInteger& CostMatrix::operator()(int row_num,
		int column_num) const { return operator()(Edge{row_num, column_num}); }
const CostMatrixInteger& CostMatrix::operator()(const Edge& e) const
{ return cost_matrix_(GetCondensedRowNum(e.u), GetCondensedColumnNum(e.v)); }

CostMatrixInteger& CostMatrix::operator()(int row_num, int column_num)
{ return operator()(Edge{row_num, column_num}); }
CostMatrixInteger& CostMatrix::operator()(const Edge& e)
{ return cost_matrix_(GetCondensedRowNum(e.u), GetCondensedColumnNum(e.v)); }


CostVector<Row> CostMatrix::GetRow(int row_num) {
	return CostVector<Row>{&cost_matrix_,
		Row{GetCondensedRowNum(row_num)}};
}

CostVector<Column> CostMatrix::GetColumn(int column_num) {
	return CostVector<Column>{&cost_matrix_,
		Column{GetCondensedColumnNum(column_num)}};
}

int CostMatrix::GetCondensedRowNum(int row_num) const {
	if (!IsRowAvailable(row_num))
	{ throw NotAvailableError{"This row number is not available"}; }
	return row_mapping_[row_num];
}

int CostMatrix::GetCondensedColumnNum(int column_num) const {
	if (!IsColumnAvailable(column_num))
	{ throw NotAvailableError{"This column number is not available"}; }
	return column_mapping_[column_num];
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

vector<int> MakeVectorMapping(const vector<bool>& available) {
	int condensed_matrix_current_cell{0};
	vector<int> mapping(available.size(), -1);

	// make mapping "actual row/column" => "condensed matrix row/column"
	for (int cell_num{0}; cell_num < int(available.size()); ++cell_num) {
		if (!available[cell_num]) { continue; }
		mapping[cell_num] = condensed_matrix_current_cell++;
	}
	return mapping;
}

void UpdateTwoSmallest(const CostMatrixInteger& current,
		pair<CostMatrixInteger, CostMatrixInteger>& two_smallest) {
	if (current < two_smallest.first) {
		two_smallest.second = two_smallest.first;
		two_smallest.first = current;
	} else if (current < two_smallest.second)
	{ two_smallest.second = current; }
}

CostMatrixInteger GetPenalty(const Edge& edge, const cmi_pair_t& penalties) {
	if (penalties.first.GetEdge() != edge) { return penalties.first; }
	return penalties.second;
}


