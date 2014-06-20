#include "tsp_solver/little_tsp/tree_node.hpp"

#include <cassert>

#include <algorithm>
#include <deque>
#include <exception>
#include <functional>
#include <iterator>
#include <limits>

#include "graph/graph.hpp"
#include "matrix.hpp"
#include "path.hpp"
#include "tsp_solver/little_tsp/cost_matrix.hpp"
#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"
#include "util.hpp"

using std::back_inserter;
using std::bind;
using std::copy_if;
using std::deque;
using std::exception;
using std::max_element;
using std::min_element;
using std::numeric_limits;
using std::ostream;
using std::pair;
using std::plus;
using std::placeholders::_1;
using std::sort;
using std::vector;

const int infinity{numeric_limits<int>::max()};

TreeNode::TreeNode(const Graph& graph) : next_edge_{-1, -1}, 
		found_lb_and_edge_{false}, has_exclude_branch_{false}, lower_bound_{0} {
	// exclude all cells along the diagonal, we don't want self-loops
	for (int diag{0}; diag < graph.GetNumVertices(); ++diag) {
		exclude_.push_back({diag, diag});
	}
}

TreeNode::TreeNode(const TreeNode& old, Edge e, bool inc) : TreeNode{old} { 
	inc ? AddInclude(e) : AddExclude(e); 
}

void TreeNode::AddInclude(const Edge& e) {
	// find the largest subtour involving the edge to add
	deque<int> subtour{e.u, e.v};
	bool found_edge{true};
	while(found_edge) {
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
Path TreeNode::GetTSPPath(const Graph& graph) const {
	// create the solution path
	Path solution;
	solution.vertices.reserve(include_.size());

	// to help find the path
	int past_vertex{0};
	int vertex{0};

	// sort the edges and then find the path through them
	vector<Edge> edges = include_;
	sort(edges.begin(), edges.end(), [](const Edge& first, const Edge& second) 
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

struct CostMatrixZero {
	Edge edge;
	int penalty;
};

bool TreeNode::CalcLBAndNextEdge(const Graph& graph) {
	// create a cost matrix from information stored in the tree node, reduce it
	// and use reduction to calculate lower bound
	CostMatrix cost_matrix{graph, include_, exclude_};
	lower_bound_ = cost_matrix.ReduceMatrix();

	// find all the zeros in the matrix
	vector<CostMatrixZero> zeros;
	for (int row{0}; row < graph.GetNumVertices(); ++row) {
		for (int column{0}; column < graph.GetNumVertices(); ++column) {
			if (!cost_matrix(row, column).IsAvailable()) { continue; }
			if (cost_matrix(row, column)() == 0) { 
				zeros.push_back({{row, column}, 0});
			}
		}
	}

	// 3 cases
	// 1. base case: 2 edges left to add
	if (graph.GetNumVertices() - include_.size() == 2) {
		assert(!zeros.empty());
		return HandleBaseCase(graph, cost_matrix, zeros[0]);
	}

	// get a penalty associated with each zero
	for (CostMatrixZero& zero : zeros) {
		// get the penalty in the row
		CostMatrix::CostRow cost_row{cost_matrix.GetRow(zero.edge.u)};
		auto row_it = min_element(cost_row.begin(), cost_row.end());

		// get the penalty in the column
		CostMatrix::CostColumn cost_column{cost_matrix.GetColumn(zero.edge.v)};
		auto col_it = min_element(cost_column.begin(), cost_column.end());

		bool found_min_col{col_it != cost_column.end()};
		bool found_min_row{row_it != cost_row.end()};
		int min_col{(*col_it)()};
		int min_row{(*row_it)()};

		// 2. case when excluding the node creates a disconnected graph
		if (found_min_col != found_min_row) {
			assert(graph.GetNumVertices() - include_.size() != 2);
			// we must choose this edge and cannot branch
			next_edge_ = zero.edge;
			has_exclude_branch_ = false;
			return true;
		}
		// 3. normal case, there is both an include and exclude branch
		else { zero.penalty = min_row + min_col; }
	}

	// set the next edge as the zero with the highest penalty
	auto max_penalty_it = max_element(zeros.begin(), zeros.end(), 
			[](CostMatrixZero& highest, CostMatrixZero& current) {
				return highest.penalty < current.penalty;
			});
	
	// for any other case, set the next edge to branch on
	// and set the flag that calculations have been completed
	next_edge_ = max_penalty_it->edge;
	found_lb_and_edge_ = true;
	return true;
}

bool TreeNode::HandleBaseCase(const Graph& graph, const CostMatrix& cost_matrix,
		const CostMatrixZero& zero) {
	// handle the base case
	AddInclude(zero.edge);
	int row{-1};
	int col{-1};

	// INVARIANT: only one valid edge left, find it
	for (int i{0}; i < graph.GetNumVertices(); ++i) {
		if (cost_matrix.IsRowAvailable(i) && i != zero.edge.u) {
			row = i;
			break;
		}
	}
	for (int j{0}; j < graph.GetNumVertices(); ++j) {
		if (cost_matrix.IsColumnAvailable(j) && j != zero.edge.v) {
			col = j;
			break;
		}
	}
	assert(row != -1 && col != -1);
	AddInclude({ row, col });

	// recalculate LB, i.e. the length of the TSP tour
	lower_bound_ = accumulate(include_.begin(), include_.end(), 0, 
			[&graph](int current_lb, Edge e) { return current_lb + graph(e); });

	found_lb_and_edge_ = true;
	return false;  // no next edge, we have a complete tour
}
