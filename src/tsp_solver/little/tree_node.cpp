#include "tsp_solver/little/tree_node.hpp"

#include <cassert>

#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <utility>

#include "graph/edge.hpp"
#include "graph/edge_cost.hpp"
#include "graph/graph.hpp"
#include "matrix.hpp"
#include "path.hpp"
#include "tsp_solver/little/cost_matrix.hpp"
#include "util.hpp"

using namespace std::rel_ops;

using std::begin;
using std::deque;
using std::end;
using std::for_each;
using std::pair;
using std::make_pair;
using std::max;
using std::max_element;
using std::numeric_limits;
using std::ostream;
using std::vector;

using cmi_pair_t = pair<EdgeCost, EdgeCost>;
using two_smallest_t = vector<cmi_pair_t>;

const int infinity{numeric_limits<int>::max()};

static void UpdateTwoSmallest(const EdgeCost& current,
		pair<EdgeCost, EdgeCost>& two_smallest);
static EdgeCost GetPenalty(const Edge& edge,
		const cmi_pair_t& penalties);
static vector<CostMatrixZero> FindZerosAndPenalties(
		const CostMatrix& cost_matrix);
static vector<CostMatrixZero> FindBaseCaseZerosAndPenalties(
		const vector<Edge>& zero_edges,
		const two_smallest_t& two_smallest_row,
		const two_smallest_t& two_smallest_column);

TreeNode::TreeNode(const Graph& costs) : graph_ptr_{&costs},
		exclude_{costs.GetNumVertices(), costs.GetNumVertices(), 0},
		next_edge_{-1, -1}, has_exclude_branch_{false}, lower_bound_{infinity} {
	// exclude all cells along the diagonal, we don't want self-loops
	for (int diag{0}; diag < graph_ptr_->GetNumVertices(); ++diag)
	{ AddExclude(Edge{diag, diag}); }
}

TreeNode TreeNode::MakeIncludeChild(const TreeNode& parent) {
	TreeNode child{parent};
	child.AddInclude(parent.GetNextEdge());
	child.ResetCalculatedState();
	return child;
}

TreeNode TreeNode::MakeExcludeChild(const TreeNode& parent) {
	TreeNode child{parent};
	child.AddExclude(parent.GetNextEdge());
	child.ResetCalculatedState();
	return child;
}

void TreeNode::AddInclude(const Edge& e) {
	ResetCalculatedState();
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
	AddExclude(Edge{subtour.back(), subtour.front()});
}

// build the TSP path once it exists
// this method will infinite loop if there is not a full path
Path TreeNode::GetTSPPath() const {
	// bucket sort the edges and then find the path through them
	vector<Edge> edges{include_};
	for_each(begin(include_), end(include_),
			[&edges](const Edge& e) { edges[e.u] = e; });

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

int TreeNode::CalculateLowerBound() const {
	return accumulate(begin(include_), end(include_), 0,
			[this](int current_lb, Edge e)
			{ return current_lb + (*graph_ptr_)(e)(); });
}

void TreeNode::ResetCalculatedState() {
	has_exclude_branch_ = false;
	next_edge_ = Edge{-1, -1};
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

bool TreeNode::CalcLBAndNextEdge() {
	// create a cost matrix from information stored in the tree node, reduce it
	// use current edges and reduced cost matrix to calculate lower bound
	CostMatrix cost_matrix{*graph_ptr_, include_, exclude_};
	lower_bound_ = cost_matrix.ReduceMatrix();
	if (lower_bound_ == infinity) { return false; }
	lower_bound_ += CalculateLowerBound();

	// find all the zeros in the matrix and copy them into the zeros vector
	vector<CostMatrixZero> zeros{FindZerosAndPenalties(cost_matrix)};

	// handle base case in a separate function
	if (cost_matrix.GetCondensedSize() == 2)
	{ return HandleBaseCase(zeros); }

	// for any other case, set the next edge as the zero with the highest
	// penalty and allow an exclude branch if the penalty is not infinite
	assert(zeros.size() == 1);
	next_edge_ = zeros[0].edge;
	has_exclude_branch_ = zeros[0].penalty != infinity;
	return true;
}

bool TreeNode::HandleBaseCase(const vector<CostMatrixZero>& zeros) {
	// find the edge with the largest penalty, remove it from zeros
	auto max_penalty_it = max_element(begin(zeros), end(zeros));
	Edge edge{max_penalty_it->edge};
	AddInclude(edge);

	// find the zero that completes the path (is not in the same row or column)
	auto last_zero_it = find_if_not(begin(zeros), end(zeros),
			[&edge](const CostMatrixZero& current) {
			return current.edge.u == edge.u || current.edge.v == edge.v; });
	assert(last_zero_it != end(zeros));
	AddInclude(last_zero_it->edge);

	// recalculate LB, i.e. the length of the TSP tour
	lower_bound_ = CalculateLowerBound();

	return false;  // no next edge, we have a complete tour
}

// Finds the zero with the highest "penalty" for exclusion.
// The penalty of any zero is defined as the amount the lower bound would
// increase if the zero were excluded from the TSP path
// Note: only returns more than one zero in the base case
vector<CostMatrixZero> FindZerosAndPenalties(const CostMatrix& cost_matrix) {
	// hold the two smallest elements in each row and column
	cmi_pair_t infinite_cmis{make_pair(EdgeCost::Infinite(),
		EdgeCost::Infinite())};
	two_smallest_t two_smallest_row(cost_matrix.GetActualSize(), infinite_cmis);
	two_smallest_t two_smallest_column(cost_matrix.GetActualSize(),
			infinite_cmis);

	// hold the zeros
	vector<Edge> zero_edges;

	// find zeros and store two smallest elements
	for (const EdgeCost& current : cost_matrix) {
		const Edge& current_edge{current.GetEdge()};
		// find zeros
		if (current() == 0) { zero_edges.push_back(current_edge); }
		// look for two smallest elements
		UpdateTwoSmallest(current, two_smallest_row[current_edge.u]);
		UpdateTwoSmallest(current, two_smallest_column[current_edge.v]);
	}

	// 3 cases
	// 1. base case: 2 edges left to add.
	// Logic is sufficiently different that it belongs in its own function
	if (cost_matrix.GetCondensedSize() == 2) {
		return FindBaseCaseZerosAndPenalties(
				zero_edges, two_smallest_row, two_smallest_column);
	}

	// hold a dummy value for max now that will be replaced on first iteration
	vector<CostMatrixZero> cost_matrix_zeros;
	cost_matrix_zeros.push_back(CostMatrixZero{Edge{-1, -1}, -1});

	for (const Edge& edge : zero_edges) {
		// get the row and column penalties
		EdgeCost row_penalty{
			GetPenalty(edge, two_smallest_row[edge.u])};
		EdgeCost column_penalty{
			GetPenalty(edge, two_smallest_column[edge.v])};

		// 2. case when excluding the node creates a disconnected graph
		// we must choose this edge and cannot branch, so return vector with
		// single element that is this zero
		if (column_penalty.IsInfinite() || row_penalty.IsInfinite())
		{ return { CostMatrixZero{edge, infinity} }; }

		// 3. normal case, there is both an include and exclude branch
		// keep only the structure with the maximum penalty
		CostMatrixZero current_zero{edge, row_penalty() + column_penalty()};
		cost_matrix_zeros[0] = max(cost_matrix_zeros[0], current_zero);
	}
	return cost_matrix_zeros;
}

// Find all the zeros and their penalties in the base case.
// They must have a zero or infinite penalty
vector<CostMatrixZero> FindBaseCaseZerosAndPenalties(
		const vector<Edge>& zero_edges,
		const two_smallest_t& two_smallest_row,
		const two_smallest_t& two_smallest_column) {

	vector<CostMatrixZero> cost_matrix_zeros;
	for (const Edge& edge : zero_edges) {
		// get the row and column penalties
		EdgeCost row_penalty{
			GetPenalty(edge, two_smallest_row[edge.u])};
		EdgeCost column_penalty{
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

// Given the cost of the current edge, check if it is one of the two smallest
// and update the two smallest accordingly.
void UpdateTwoSmallest(const EdgeCost& current,
		pair<EdgeCost, EdgeCost>& two_smallest) {
	if (current < two_smallest.first) {
		two_smallest.second = two_smallest.first;
		two_smallest.first = current;
	} else if (current < two_smallest.second)
	{ two_smallest.second = current; }
}

// Calculates the penalty for an edge by returning the difference between it and
// the next smallest edge in the row as stored in penalties.
EdgeCost GetPenalty(const Edge& edge, const cmi_pair_t& penalties) {
	if (penalties.first.GetEdge() != edge) { return penalties.first; }
	return penalties.second;
}

