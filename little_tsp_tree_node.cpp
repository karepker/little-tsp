#include "little_tsp_tree_node.hpp"

#include <cassert>

#include <algorithm>
#include <deque>
#include <exception>
#include <iterator>
#include <limits>

#include "graph.hpp"
#include "little_tsp_cost_matrix_integer.hpp"
#include "matrix.hpp"
#include "path.hpp"
#include "util.hpp"

using std::back_inserter;
using std::copy_if;
using std::deque;
using std::exception;
using std::max_element;
using std::numeric_limits;
using std::ostream;
using std::pair;
using std::sort;
using std::vector;

const int infinity{numeric_limits<int>::max()};

TreeNode::TreeNode(const TreeNode& old, Edge e, bool inc) : TreeNode{old}
	{ inc ? AddInclude(e) : AddExclude(e); }

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

void TreeNode::AddExclude(const Edge& e) { exclude_.push_back(e); }

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

bool TreeNode::CalcLBAndNextEdge(const Graph& graph) {
	// create a cost matrix from information stored in the tree node, reduce it
	// and use reduction to calculate lower bound
	CostMatrix cost_matrix{graph, include_, exclude_};
	lower_bound_ = cost_matrix.ReduceMatrix();

	// find all the zeros in the matrix
	vector<Edge> zeros;
	for (int row{0}; row < graph.GetNumVertrowces(); ++row) {
		for (int column{0}; column < graph.GetNumVertcolumnces(); ++column) {
			if (!cost_matrix(row, column).IsAvailable()) { continue; }
			if (cost_matrix(row, column)() == 0) { 
				zeros.push_back({row, column});
			}
		}
	}

	// get a penalty associated with each zero
	vector<int> penalties(zeros.size());
	int counter{0};
	for (const Edge zero : zeros) {
		bool found_min_col{false};
		bool found_min_row{false};
		int min_col{infinity};
		int min_row{infinity};

		// check for largest distance in row
		for (int i = 0; i < graph.GetNumVertices(); ++i) {
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
