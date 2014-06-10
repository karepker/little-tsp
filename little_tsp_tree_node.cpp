#include "little_tsp_tree_node.hpp"

#include <cassert>

#include <algorithm>
#include <deque>
#include <exception>
#include <limits>

#include "graph.hpp"
#include "little_tsp_cost_matrix_integer.hpp"
#include "matrix.hpp"
#include "path.hpp"

using std::deque;
using std::exception;
using std::max_element;
using std::numeric_limits;
using std::ostream;
using std::pair;
using std::sort;
using std::vector;

const int infinity{numeric_limits<int>::max()};

class NotAvailableError : public exception {
public:
	NotAvailableError(const char* msg) : msg_{msg} {}
	const char* what() const noexcept override { return msg_; }

private:
	const char* msg_;
};

// information about the useable matrix
// temporary that is used to help build a TreeNode
class CostMatrix {
public:
	CostMatrix(const Graph& graph, const vector<Edge>& include, 
			const vector<Edge>& exclude) : 
			cost_matrix_{graph.GetNumVertices(), graph.GetNumVertices()},
		
	// reduce the matrix by storing information about reduction
	int ReduceRow(int i, const Graph& graph, const TreeNode& p);
	int ReduceCol(int j, const Graph& graph, const TreeNode& p);
	int ReduceMatrix(const Graph& graph, const TreeNode& p);

	// getters
	int GetRowReduction(int row) const;
	int GetColumnReduction(int column) const;	
	int IsRowAvailable(int row) const;	
	int IsColumnAvailable(int column) const;

	// setters
	void SetRowUnavailable(int row);
	void SetColumnUnavailable(int column);

	// these classes need to be able to share data with each other because they
	// are very intimately related
	class CostVector;
	class CostRow;
	class CostColumn;
	friend class CostVector;
	friend class CostRow;
	friend class CostColumn;

	// encapsulation of cost information about either a single row or a column
	// defines iterator which allows easier traversal and more use of STL
	class CostVector {
	public:
		CostVector(const Graph& graph, const CostMatrix& cost_matrix, 
				const Matrix<bool>& infinite) : cost_matrix_{cost_matrix}, 
			graph_{graph}, infinite_{infinite} {}

		int operator[](int cell_num) const;

		// we need access to the protected interface in cost vector and the
		// subscripting operation
		class Iterator;
		friend class Iterator;

		class Iterator {
		public:
			Iterator(const CostVector& cost_vector) :
				cost_vector_{cost_vector}, traversing_cell_num_{0} {}

			int operator*() { return cost_vector_[traversing_cell_num_]; }

			Iterator operator++(int);
			Iterator& operator++();

			friend class CostVector;

		private:
			Iterator(const CostVector& cost_vector, int cell_num) :
				cost_vector_{cost_vector}, traversing_cell_num_{cell_num} {}

			const CostVector& cost_vector_;
			int traversing_cell_num_;  // changes as iterator moves
		};

		Iterator begin() { return Iterator{*this}; }
		Iterator end() { return Iterator{*this, graph_.GetNumVertices() + 1}; }

	protected:
		// define interface for derived classes because CostVector doesn't know
		// if it's a row or column
		virtual bool CellAvailable(int cell_num) const = 0;
		virtual int GetRow(int cell_num) const = 0;
		virtual int GetColumn(int cell_num) const = 0;

		// this is usually bad design, but it exposes a needed interface
		const CostMatrix& cost_matrix_;

	private:
		const Graph& graph_;
		const Matrix<bool>& infinite_;
	};

	class CostColumn : public CostVector {
	public:
		CostColumn(const Graph& graph, const CostMatrix& cost_matrix_, 
				Matrix<bool> infinite, int column_num) :
				CostVector{graph, cost_matrix_, infinite}, 
				column_num_{column_num} {
			if (cost_matrix_.IsColumnAvailable(column_num_)) { 
				throw NotAvailableError{"That column is not available"};
			}
		}

	private:
		bool CellAvailable(int cell_num) const override { 
			return cost_matrix_.IsRowAvailable(cell_num); 
		}
		int GetRow(int cell_num) const override { return cell_num; }
		int GetColumn(int) const override { return column_num_; }

		int column_num_;
	};

	class CostRow : public CostVector {
	public:
		CostRow(const Graph& graph, const CostMatrix& cost_matrix_, 
				Matrix<bool> infinite, int row_num) :
				CostVector{graph, cost_matrix_, infinite}, row_num_{row_num} {
			if (cost_matrix_.IsRowAvailable(row_num_)) {
				throw NotAvailableError{"That row is not available"};
			}
		}

	private:
		bool CellAvailable(int cell_num) const override { 
			return cost_matrix_.IsColumnAvailable(cell_num); 
		}
		int GetColumn(int cell_num) const override { return cell_num; }
		int GetRow(int) const override { return row_num_; }

		int row_num_;
	};
	
private:
	Matrix<CostMatrixInteger> cost_matrix_;
};

using CostIterator = CostMatrix::CostVector::Iterator;

int CostMatrix::CostVector::operator[](int cell_num) const {
	int row_num{GetRow(cell_num)};
	int column_num{GetColumn(cell_num)};

	if (!CellAvailable(cell_num)) {
		throw NotAvailableError{"That index is not available!"};
	} else if (infinite_(row_num, column_num)) { return infinity; }

	return graph_(row_num, column_num) - cost_matrix_.GetRowReduction(row_num) - 
		cost_matrix_.GetColumnReduction(column_num);
}

CostIterator CostIterator::operator++(int) {
	int current_traversing_cell_num{traversing_cell_num_};
	while (!cost_vector_.CellAvailable(++traversing_cell_num_));
	return Iterator{cost_vector_, current_traversing_cell_num};
}

CostIterator& CostIterator::operator++() {
	while (!cost_vector_.CellAvailable(++traversing_cell_num_));
	return *this;
}

// these methods will fail if there's no min
int CostMatrix::ReduceRow(int i, const Graph& graph, const TreeNode& p) {
	int smallest_elt{0};

	// find the smallest element in the row
	if (row_available_[i])
	{
		smallest_elt = infinity;
		for (int j = 0; j < graph.GetNumVertices(); ++j)
		{
			if (column_available_[j] && !p.IsInfinite(i, j))
			{
				int current{graph(i, j) - 
					row_reductions_[i] - column_reductions_[j]};
				if (current < smallest_elt) { smallest_elt = current; }
			}
		}
	}

	// reduce each element in the row by the smallest elt
	row_reductions_[i] += smallest_elt;
	return smallest_elt;
}

// these methods will fail if there's no min
int CostMatrix::ReduceCol(int j, const Graph& graph, const TreeNode& p)
{
	int smallest_elt{0};

	// find the smallest element in the row
	if (column_available_[j])
	{
		smallest_elt = infinity;
		for (int i = 0; i < graph.GetNumVertices(); ++i)
		{
			if (row_available_[i] && !p.IsInfinite(i, j))
			{
				int current = graph(i, j) - 
					row_reductions_[i] - column_reductions_[j];
				if (current < smallest_elt) { smallest_elt = current; }
			}
		}
	}

	// reduce each element in the row by the smallest elt
	column_reductions_[j] += smallest_elt;
	return smallest_elt;
}

int CostMatrix::ReduceMatrix(const Graph& graph, const TreeNode& p) {
	int dec{0};

	// reduce the rows
	for (int i{0}; i < graph.GetNumVertices(); ++i) { 
		dec += ReduceRow(i, graph, p); 
	}

	// reduce the columns
	for (int j{0}; j < graph.GetNumVertices(); ++j) { 
		dec += ReduceCol(j, graph, p); 
	}

	// return the total reduction
	return dec;
}

// constructor for the "root" TreeNode
TreeNode::TreeNode(const Graph& costs) : has_exclude_branch_{true}, 
		lower_bound_{infinity} {
	// set all elements on the diagonal as infinite
	for (int diag{0}; diag < costs.GetNumVertices(); ++diag)
		{ SetInfinite(diag, diag); }
}

TreeNode::TreeNode(const TreeNode& old, Edge e, bool inc) : TreeNode{old}
	{ inc ? AddInclude(e) : AddExclude(e); }

void TreeNode::AddInclude(const Edge& e) {
	// find the largest subtour involving the edge to add
	deque<int> subtour{ e.u, e.v };
	bool found_edge{true};
	while(found_edge)
	{
		found_edge = false;
		for (const Edge& check : include_)
		{
			// determine if "check" goes before in a subtour
			if (check.v == subtour.front())
			{
				subtour.push_front(check.v);
				subtour.push_front(check.u);
				found_edge = true;
				break;
			}

			// determine if "check" goes after in a subtour
			else if (check.u == subtour.back())
			{
				subtour.push_back(check.u);
				subtour.push_back(check.v);
				found_edge = true;
				break;
			}
		}
	}

	// add the edge onto include
	include_.push_back(e);

	// make the ends of the longest subtour infinite
	exclude_.push_back({subtour.back(), subtour.front()});
}

void TreeNode::AddExclude(const Edge& e) { exclude_.push_back(e); }

void TreeNode::SetAvailAndLB(const Graph& graph, CostMatrix& info) {
	// reset lower bound
	lower_bound_ = 0;

	// start its calculation using the cost of the includes
	for (const Edge& e : include_) {
		lower_bound_ += graph(e.u, e.v);
		info.SetRowUnavailable(e.u);
		info.SetColumnUnavailable(e.v);
	}
}

// build the TSP path once it exists
// this method will infinite loop if there is not a full path
Path TreeNode::GetTSPPath(const Graph& graph) const {
	// create the solution path
	Path solution;
	solution.vertices.reserve(include_.size());

	// to help find the path
	int past_vertex{0};
	int vertex{0};

	// sort the edges and then find the path through them
	vector<Edge> edges = include_;
	sort(edges.begin(), edges.end(), 
			[](const Edge& first, const Edge& second) 
			{ return first.u < second.u; });

	for (int i{0}; i < int(edges.size()); ++i) {
		// push the next vertex on to the path
		solution.vertices.push_back(vertex);
		vertex = edges[vertex].v;

		// incremement the length of the path
		solution.length += graph(past_vertex, vertex);
		past_vertex = vertex;
	}

	// finish the path
	assert(vertex == 0);
	solution.length += graph(past_vertex, vertex);

	return solution;
}

ostream& operator<<(ostream& os, const TreeNode& p) {
	os << "{ ";
	for (const Edge& e : p.include_) { os << "(" << e.u << " " << e.v << ") "; }
	os << " } ";
	return os;
}

bool TreeNode::CalcLBAndNextEdge(const Graph& graph) {
	// information about the useable matrix
	CostMatrix info(graph.GetNumVertices());

	// calculate the initial LB from edges already included
	// also, set any edges that would create a subtour as infinite
	SetAvailAndLB(graph, info);

	// reduce the matrix, increment the lower bound from the reduction
	lower_bound_ += info.ReduceMatrix(graph, *this);

	// find all the zeros in the matrix
	vector<Edge> zeros;
	for (int i{0}; i < graph.GetNumVertices(); ++i)
	{
		if (info.IsRowAvailable(i)) {
			for (int j{0}; j < graph.GetNumVertices(); ++j) {
				if (info.IsColumnAvailable(j) && !IsInfinite(i, j)) {
					if (graph(i, j) - info.GetRowReduction(i) - 
						info.GetColumnReduction(j) == 0) {
						zeros.push_back({ i, j });
					}
				}
			}
		}
	}
	
	
	// get a penalty associated with each zero
	vector<int> penalties(zeros.size());
	int counter{0};
	for (const Edge zero : zeros)
	{
		bool found_min_col{false};
		bool found_min_row{false};
		int min_col{infinity};
		int min_row{infinity};

		// check for largest distance in row
		for (int i = 0; i < graph.GetNumVertices(); ++i)
		{
			// INVARIANT: column is always available
			if (info.IsRowAvailable(i) && !IsInfinite(i, zero.v) && 
					i != zero.u && graph(i, zero.v) - info.GetRowReduction(i) - 
					info.GetColumnReduction(zero.v) < min_row) {
				min_row = graph(i, zero.v) - info.GetRowReduction(i) - 
					info.GetColumnReduction(zero.v);
				found_min_row = true;
			}
		}

		// check largest distance in col
		for (int j = 0; j < graph.GetNumVertices(); ++j) {
			// INVARIANT: row is always available
			if (info.IsColumnAvailable(j) && !IsInfinite(zero.u, j) && 
					zero.v != j && graph(zero.u, j) - 
					info.GetRowReduction(zero.u) - 
					info.GetColumnReduction(j) < min_col) {
				min_col = graph(zero.u, j) - info.GetRowReduction(zero.u) -
					info.GetColumnReduction(j);
				found_min_col = true;
			}
		}

		// 3 cases
		// 1. base case: 2 edges left to add
		if (graph.GetNumVertices() - include_.size() == 2) {
			penalties[counter++] = infinity;
			break;
		}

		// 2. case when excluding the node creates a disconnected graph
		else if (found_min_col != found_min_row && 
				graph.GetNumVertices() - include_.size() != 2) {
			// we must choose this edge and cannot branch
			next_edge_ = zero;
			has_exclude_branch_ = false;
			return true;
		}
		// 3. normal case, there is both an include and exclude branch
		else { penalties[counter++] = min_row + min_col; }
	}

	// set the next edge as the zero with the highest penalty
	auto max_penalty_it = max_element(penalties.begin(), penalties.end());
	int index(max_penalty_it - penalties.begin());

	// handle the base case
	if (graph.GetNumVertices() - include_.size() == 2) {
		AddInclude(zeros[index]);
		info.SetRowUnavailable(zeros[index].u);
		info.SetColumnUnavailable(zeros[index].v);
		int row{0};
		int col{0};

		// INVARIANT: only one valid edge left, find it
		for (int i{0}; i < graph.GetNumVertices(); ++i) {
			if (info.IsRowAvailable(i)) {
				row = i;
				break;
			}
		}
		for (int j{0}; j < graph.GetNumVertices(); ++j) {
			if (info.IsColumnAvailable(j)) {
				col = j;
				break;
			}
		}
		AddInclude({ row, col });

		// recalculate LB, i.e. the length of the TSP tour
		SetAvailAndLB(graph, info);
		found_lb_and_edge_ = true;
		return false;  // no next edge, we have a complete tour
	}

	// for any other case, set the next edge to branch on
	// and set the flag that calculations have been completed
	next_edge_ = zeros[index];
	found_lb_and_edge_ = true;
	return true;
}
