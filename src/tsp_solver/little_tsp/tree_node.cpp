#include "tsp_solver/little_tsp/tree_node.hpp"

#include <cassert>

#include <algorithm>
#include <deque>
#include <exception>
#include <iterator>
#include <limits>

#include "graph/graph.hpp"
#include "matrix.hpp"
#include "path.hpp"
#include "tsp_solver/little_tsp/cost_matrix.hpp"
#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"
#include "util.hpp"

using std::back_inserter;
using std::copy_if;
using std::deque;
using std::exception;
using std::for_each;
using std::max_element;
using std::min_element;
using std::numeric_limits;
using std::ostream;
using std::vector;

const int infinity{numeric_limits<int>::max()};

TreeNode::TreeNode(const Graph& costs) : graph_ptr_{&costs}, 
		next_edge_{-1, -1}, found_lb_and_edge_{false}, 
		has_exclude_branch_{false}, lower_bound_{infinity} {
	// exclude all cells along the diagonal, we don't want self-loops
	for (int diag{0}; diag < graph_ptr_->GetNumVertices(); ++diag) {
		exclude_.push_back({diag, diag});
	}
}

void TreeNode::AddInclude(const Edge& e) {
	// find the largest subtour involving the edge to add
	deque<int> subtour{e.u, e.v};
	bool found_edge{true};
	while (found_edge) {
		found_edge = false;
		for (const Edge& check : include_) {
			// determine if "check" goes before in a subtour
			if (check.v == subtour.front()) {
				subtour.push_front(check.v);
				subtour.push_front(check.u);
				found_edge = true;
				break;
			}

			// determine if "check" goes after in a subtour
			else if (check.u == subtour.back()) {
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

// build the TSP path once it exists
// this method will infinite loop if there is not a full path
Path TreeNode::GetTSPPath() const {
	// bucket sort the edges and then find the path through them
	vector<Edge> edges{include_};
	for_each(include_.begin(), include_.end(), [&edges](const Edge& e) 
			{ edges[e.u] = e; });

	// to help find the path
	Path solution;
	int vertex{0};

	for (int i{0}; i < int(edges.size()); ++i) {
		// push the next vertex on to the path
		solution.vertices.push_back(vertex);
		vertex = edges[vertex].v;
	}

	// set the path length
	assert(vertex == 0);
	solution.length = CalculateLowerBound();

	return solution;
}

ostream& operator<<(ostream& os, const TreeNode& p) {
	os << "{ ";
	for (const Edge& e : p.include_) { os << "(" << e.u << " " << e.v << ") "; }
	os << " } ";
	return os;
}

struct CostMatrixZero {
	Edge edge;
	int penalty;
	bool operator<(const CostMatrixZero& other) const
	{ return penalty < other.penalty; }
};

class SetEdgeInfiniteTemporarily {
public:
	SetEdgeInfiniteTemporarily(CostMatrix& cost_matrix, const Edge& e) : 
			cost_matrix_{cost_matrix}, edge_{e}, finite_zero_{cost_matrix(e)} {
		cost_matrix_(e).SetInfinite();
	}
	~SetEdgeInfiniteTemporarily() { cost_matrix_(edge_) = finite_zero_; }

private:
	CostMatrix& cost_matrix_;	
	Edge edge_;	
	CostMatrixInteger finite_zero_;
};

bool TreeNode::CalcLBAndNextEdge() {
	// create a cost matrix from information stored in the tree node, reduce it
	// use current edges and reduced cost matrix to calculate lower bound
	CostMatrix cost_matrix{*graph_ptr_, include_, exclude_};
	lower_bound_ = CalculateLowerBound();
	lower_bound_ += cost_matrix.ReduceMatrix();

	// find all the zeros in the matrix and copy them into the zeros vector
	vector<CostMatrixZero> zeros;
	vector<CostMatrixInteger> zero_cmis;
	copy_if(cost_matrix.begin(), cost_matrix.end(), back_inserter(zero_cmis),
		[](const CostMatrixInteger& cmi) { return cmi() == 0; });
	transform(zero_cmis.begin(), zero_cmis.end(), back_inserter(zeros), 
		[](const CostMatrixInteger& cmi) 
		{ return CostMatrixZero{cmi.GetEdge(), 0}; });
	assert(!zeros.empty());

	// get a penalty associated with each zero
	for (CostMatrixZero& zero : zeros) {
		// set the cell corresponding to the zero as infinite in the cost matrix
		// so it is not chosen as the minimum element
		SetEdgeInfiniteTemporarily infinite_edge{cost_matrix, zero.edge};

		// get the penalty in the row and column
		CostMatrix::CostRow cost_row{cost_matrix.GetRow(zero.edge.u)};
		CostMatrix::CostColumn cost_column{cost_matrix.GetColumn(zero.edge.v)};

		auto row_it = min_element(cost_row.begin(), cost_row.end());
		auto col_it = min_element(cost_column.begin(), cost_column.end());
		assert(col_it != cost_column.end() && row_it != cost_row.end());

		// 3 cases
		// 1. base case: 2 edges left to add. We need to know if penalty is zero
		// or infinite so we can choose the two edges with the highest penalties
		if (graph_ptr_->GetNumVertices() - include_.size() == 2) {
			if (col_it->IsInfinite() || row_it->IsInfinite()) { 
				zero.penalty = infinity;
			} else { 
				assert((*col_it)() == 0 && (*row_it)() == 0);
				zero.penalty = 0;
			}
			continue;
		}

		// 2. case when excluding the node creates a disconnected graph_
		if (col_it->IsInfinite() != row_it->IsInfinite()) {
			assert(graph_ptr_->GetNumVertices() - include_.size() != 2);
			// we must choose this edge and cannot branch
			next_edge_ = zero.edge;
			has_exclude_branch_ = false;
			return true;
		}

		// 3. normal case, there is both an include and exclude branch
		zero.penalty = (*col_it)() + (*row_it)();
	}

	// finish handling base case in a separate function
	if (graph_ptr_->GetNumVertices() - include_.size() == 2) {
		return HandleBaseCase(cost_matrix, zeros);
	}

	// set the next edge as the zero with the highest penalty
	auto max_penalty_it = max_element(zeros.begin(), zeros.end());
		
	// for any other case, set the next edge to branch on
	// and set the flag that calculations have been completed
	next_edge_ = max_penalty_it->edge;
	found_lb_and_edge_ = true;
	has_exclude_branch_ = true;
	return true;
}

bool TreeNode::HandleBaseCase(const CostMatrix& cost_matrix,
		const vector<CostMatrixZero>& zeros) {
	// find the edge with the largest penalty, remove it from zeros
	auto max_penalty_it = max_element(zeros.begin(), zeros.end());
	Edge edge = max_penalty_it->edge;
	AddInclude(edge);

	// find the zero that completes the path (is not in the same row or column)
	auto last_zero_it = find_if_not(zeros.begin(), zeros.end(), 
			[&edge](const CostMatrixZero& current) { 
			return current.edge.u == edge.u || current.edge.v == edge.v; });
	assert(last_zero_it != zeros.end());
	AddInclude(last_zero_it->edge);

	// recalculate LB, i.e. the length of the TSP tour
	lower_bound_ = CalculateLowerBound();

	has_exclude_branch_ = false;
	found_lb_and_edge_ = true;
	return false;  // no next edge, we have a complete tour
}

int TreeNode::CalculateLowerBound() const {
	return accumulate(include_.begin(), include_.end(), 0,
		[this](int current_lb, Edge e) { 
		return current_lb + (*graph_ptr_)(e); 
		});
}
