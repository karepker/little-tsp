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
#include "tsp_solver/little_tsp/util.hpp"

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
		next_edge_{-1, -1}, has_exclude_branch_{false}, lower_bound_{infinity} {
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

bool TreeNode::CalcLBAndNextEdge() {
	// create a cost matrix from information stored in the tree node, reduce it
	// use current edges and reduced cost matrix to calculate lower bound
	CostMatrix cost_matrix{*graph_ptr_, include_, exclude_};
	lower_bound_ = CalculateLowerBound();
	lower_bound_ += cost_matrix.ReduceMatrix();

	// find all the zeros in the matrix and copy them into the zeros vector
	vector<CostMatrixZero> zeros{cost_matrix.FindZerosAndPenalties()};

	// handle base case in a separate function
	if (cost_matrix.Size() == 2) { return HandleBaseCase(cost_matrix, zeros); }

	// for any other case, set the next edge as the zero with the highest 
	// penalty and allow an exclude branch if the penalty is not infinite
	assert(zeros.size() == 1);
	next_edge_ = zeros[0].edge;
	has_exclude_branch_ = zeros[0].penalty != infinity;
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
	return false;  // no next edge, we have a complete tour
}

int TreeNode::CalculateLowerBound() const {
	return accumulate(include_.begin(), include_.end(), 0,
		[this](int current_lb, Edge e) {
		return current_lb + (*graph_ptr_)(e);
		});
}
