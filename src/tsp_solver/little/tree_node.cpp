#include "tsp_solver/little/tree_node.hpp"

#include <cassert>

#include <algorithm>
#include <deque>
#include <iterator>
#include <utility>
#include <limits>

#include "graph/edge_cost.hpp"
#include "graph/graph.hpp"
#include "matrix.hpp"
#include "path.hpp"
#include "tsp_solver/little/cost_matrix.hpp"
#include "util.hpp"

using std::back_inserter;
using std::deque;
using std::exception;
using std::for_each;
using std::pair;
using std::make_pair;
using std::max;
using std::max_element;
using std::numeric_limits;
using std::ostream;
using std::vector;

using cmi_pair_t = std::pair<EdgeCost, EdgeCost>;
using two_smallest_t = std::vector<cmi_pair_t>;

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
	AddExclude(Edge{subtour.back(), subtour.front()});
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

int TreeNode::CalculateLowerBound() const {
	return accumulate(include_.begin(), include_.end(), 0,
			[this](int current_lb, Edge e)
			{ return current_lb + (*graph_ptr_)(e)(); });
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
	lower_bound_ = CalculateLowerBound();
	lower_bound_ += cost_matrix.ReduceMatrix();

	// find all the zeros in the matrix and copy them into the zeros vector
	vector<CostMatrixZero> zeros{FindZerosAndPenalties(cost_matrix)};

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
	Edge edge{max_penalty_it->edge};
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
	for (int i{0}; i < cost_matrix.GetActualSize(); ++i) {
		if (!cost_matrix.IsRowAvailable(i)) { continue; }
		for (int j{0}; j < cost_matrix.GetActualSize(); ++j) {
			if (!cost_matrix.IsColumnAvailable(j)) { continue; }
			EdgeCost current{cost_matrix(i, j)};
			const Edge& current_edge{current.GetEdge()};
			assert(i == current_edge.u && j == current_edge.v);
			// find zeros
			if (current() == 0) { zero_edges.push_back(current_edge); }
			// look for two smallest elements
			UpdateTwoSmallest(current, two_smallest_row[current_edge.u]);
			UpdateTwoSmallest(current, two_smallest_column[current_edge.v]);
		}
	}

	// 3 cases
	// 1. base case: 2 edges left to add. 
	// Logic is sufficiently different that it belongs in its own function 
	if (cost_matrix.Size() == 2) {
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

void UpdateTwoSmallest(const EdgeCost& current,
		pair<EdgeCost, EdgeCost>& two_smallest) {
	if (current < two_smallest.first) {
		two_smallest.second = two_smallest.first;
		two_smallest.first = current;
	} else if (current < two_smallest.second)
	{ two_smallest.second = current; }
}

EdgeCost GetPenalty(const Edge& edge, const cmi_pair_t& penalties) {
	if (penalties.first.GetEdge() != edge) { return penalties.first; }
	return penalties.second;
}
