#include "little_tsp_tree_node.hpp"

#include <cassert>

#include <algorithm>
#include <deque>
#include <limits>

#include "graph.hpp"
#include "matrix.hpp"
#include "path.hpp"

using std::vector;
using std::pair;
using std::deque;
using std::numeric_limits;
using std::max_element;
using std::sort;
using std::ostream;

const int infinity{numeric_limits<int>::max()};

// information about the useable matrix
// temporary that is used to help build a TreeNode
class CostMatrix {
public:
	CostMatrix(int size) : row_reductions_(size, 0), 
		column_reductions_(size, 0), row_available_(size, true), 
		column_available_(size, true) {};

	// reduce the matrix by storing information about reduction
	int ReduceRow(int i, const Graph& graph, const TreeNode& p);
	int ReduceCol(int j, const Graph& graph, const TreeNode& p);
	int ReduceMatrix(const Graph& graph, const TreeNode& p);

private:
	// whether the rows are available
	std::vector<int> row_reductions_;
	std::vector<int> column_reductions_;

	// whether the columns are available
	std::vector<bool> row_available_;
	std::vector<bool> column_available_;
};



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

int CostMatrix::ReduceMatrix(const Graph& c, const TreeNode& p) {
	int dec{0};

	// reduce the rows
	for (int i{0}; i < graph.GetNumVertices(); ++i) { 
		dec += ReduceRow(i, c, p); 
	}

	// reduce the columns
	for (int j{0}; j < graph.GetNumVertices(); ++j) { 
		dec += ReduceCol(j, c, p); 
	}

	// return the total reduction
	return dec;
}

// constructor for the "root" TreeNode
TreeNode::TreeNode(const Graph& costs) :
		infinite_{costs.GetNumVertices() * costs.GetNumVertices()}, 
		has_exclude_branch_{true}, lower_bound_{0} {
	// set all elements on the diagonal as infinite
	for (int diag{0}; diag < costs.size; ++diag) { SetInfinite(diag, diag); }
}

TreeNode::TreeNode(const TreeNode& old, Edge e, bool inc) : TreeNode(old) {
	inc ? AddInclude(e) : AddExclude(e);
}

TreeNode::PathInfo() : both_branches_{true}, lower_bound_{infinity} {}

void TreeNode::AddInclude(const Edge& e) {
	// find the largest subtour involving the edge to add
	deque<int> subtour{ e.u, e.v };
	bool found_edge{true};
	while(found_edge)
	{
		found_edge = false;
		for (const Edge& check : include)
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
	include.push_back(e);

	// make the ends of the longest subtour infinite
	SetInfinite(subtour.back(), subtour.front());
}

void TreeNode::AddExclude(const Edge& e) { SetInfinite(e.u, e.v); }

void TreeNode::SetAvailAndLB(const Graph& graph, CostMatrix& info) {
	// reset lower bound
	lower_bound_ = 0;

	// start its calculation using the cost of the includes
	for (const Edge& e : include)
	{
		lower_bound_ += graph(e.u, e.v);
		info.row_available_[e.u] = false;
		info.column_available_[e.v] = false;
	}
}

// build the TSP path once it exists
// this method will infinite loop if there is not a full path
Path TreeNode::GetTSPPath(const Graph& graph) const
{
	// create the solution path
	Path solution;
	solution.vertices.reserve(include.size());

	// to help find the path
	int past_vertex{0};
	int vertex{0};

	// sort the edges and then find the path through them
	vector<Edge> edges = include;
	sort(edges.begin(), edges.end(), 
			[](const Edge& first, const Edge& second) { first.u < second.u; });

	for (int i = 0; i < edges.size(); ++i)
	{
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

ostream& operator<<(ostream& os, const TreeNode& p)
{
	os << "{ ";
	for (const Edge& e : p.include) { os << "(" << e.u << " " << e.v << ") "; }
	os << " } ";
	return os;
}

void TreeNode::CalcLBAndNextEdge(const Graph& graph) 
{
	// information about the useable matrix
	CostMatrix info(graph.GetNumVertices);

	// calculate the initial LB from edges already included
	// also, set any edges that would create a subtour as infinite
	SetAvailAndLB(graph, info);

	// reduce the matrix, increment the lower bound from the reduction
	lower_bound_ += info.ReduceMatrix(graph, *this);

	// find all the zeros in the matrix
	vector<Edge> zeros;
	for (int i{0}; i < graph.GetNumVertices(); ++i)
	{
		if (info.row_available_[i])
		{
			for (int j{0}; j < graph.GetNumVertices(); ++j)
			{
				if (info.column_available_[j] && !IsInfinite(i, j))
				{
					if (graph(i, j) - info.row_reductions_[i] - 
						info.column_reductions_[j] == 0)
					{
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
			if (info.row_available_[i] && 
				!IsInfinite(i, zero.v) && 
				i != zero.u && graph(i, zero.v) - 
				info.row_reductions_[i] - info.column_reductions_[zero.v] < min_row)
			{
				min_row = graph(i, zero.v) - info.row_reductions_[i] - 
					info.column_reductions_[zero.v];
				found_min_row = true;
			}
		}

		// check largest distance in col
		for (int j = 0; j < graph.GetNumVertices(); ++j)
		{
			// INVARIANT: row is always available
			if (info.column_available_[j] && !IsInfinite(zero.u, j) && 
				zero.v != j && graph(zero.u, j) - info.row_reductions_[zero.u] 
				- info.column_reductions_[j] < min_col)
			{
				min_col = graph(zero.u, j) - info.row_reductions_[zero.u] -
					info.column_reductions_[j];
				found_min_col = true;
			}
		}

		// 3 cases
		// 1. base case
		if (graph.GetNumVertices() - include.size() == 2) {
			penalties[counter++] = infinity;
			break;
		}

		// 2. case when excluding the node creates a disconnected graph
		else if (found_min_col != found_min_row && 
				graph.GetNumVertices() - include.size() != 2) {
			// we must choose this edge and cannot branch
			next = zero;
			both_branches_ = false;
			return;
		}
		// 3. normal case, there is both an include and exclude branch
		else { penalties[counter++] = min_row + min_col; }
	}

	// set the next edge as the zero with the highest penalty
	auto max_penalty_it = max_element(penalties.begin(), penalties.end());
	int index{max_penalty_it - penalties.begin()};

	// handle the base case
	if (graph.GetNumVertices() - include.size() == 2) {
		AddInclude(zeros[index]);
		info.row_available_[zeros[index].u] = false;
		info.column_available_[zeros[index].v] = false;
		int row{0};
		int col{0};

		// INVARIANT: only one valid edge left, find it
		for (int i{0}; i < info.row_available_.size(); ++i) {
			if (info.row_available_[i])
			{
				row = i;
				break;
			}
		}
		for (int j{0}; j < info.column_available_.size(); ++j) {
			if (info.column_available_[j])
			{
				col = j;
				break;
			}
		}
		AddInclude({ row, col });
		// recalculate LB, the length of the TSP tour
		SetAvailAndLB(graph, info);
		throw NoNextEdge{};
	}

	// for any other case, set the next edge to branch on
	// and set the flag that calculations have been completed
	next = zeros[index];
	found_lb_and_edge_ = true;
} 
