// INCLUDES
// STL
#include <deque>
#include <limits>
#include <algorithm>
// Project
#include "pathinfo.hpp"

using std::vector;
using std::pair;
using std::deque;
using std::numeric_limits;
using std::max_element;
using std::sort;
using std::ostream;

namespace {
	const unsigned int infinity = numeric_limits<unsigned int>::max();
}

struct EdgeComp
{
	bool operator()(const Edge& first, const Edge& second)
	{ return first.first < second.first; };
};

// these methods will fail if there's no min
unsigned int MatrixInfo::reduceRow(unsigned int i, 
	const struct AdjMat& c,	const struct PathInfo& p) 
{
	unsigned int smallestElt = 0;

	// find the smallest element in the row
	if(this->rowAvail[i])
	{
		smallestElt = infinity;
		for(unsigned int j = 0; j < c.size; ++j)
		{
			if(this->colAvail[j] && !p.isInfinite(i, j))
			{
				unsigned int current = c(i, j) -
					this->rowReds[i] - this->colReds[j];
				if(current < smallestElt)
				{
					smallestElt = current;
				}
			}
		}
	}

	// reduce each element in the row by the smallest elt
	this->rowReds[i] += smallestElt;
	return smallestElt;
}

// these methods will fail if there's no min
unsigned int MatrixInfo::reduceCol(unsigned int j, 
	const struct AdjMat& c, const struct PathInfo& p)
{
	unsigned int smallestElt = 0;

	// find the smallest element in the row
	if(this->colAvail[j])
	{
		smallestElt = infinity;
		for(unsigned int i = 0; i < c.size; ++i)
		{
			if(this->rowAvail[i] && !p.isInfinite(i, j))
			{
				unsigned int current = c(i, j) - 
					this->rowReds[i] - this->colReds[j];
				if(current < smallestElt)
				{
					smallestElt = current;
				}
			}
		}
	}

	// reduce each element in the row by the smallest elt
	this->colReds[j] += smallestElt;
	return smallestElt;
}

unsigned int MatrixInfo::reduceMatrix(const struct AdjMat& c,
	const struct PathInfo& p)
{
	unsigned int dec = 0;

	// reduce the rows
	for(unsigned int i = 0; i < c.size; ++i)
	{
		dec += this->reduceRow(i, c, p);
	}

	// reduce the columns
	for(unsigned int j = 0; j < c.size; ++j)
	{
		dec += this->reduceCol(j, c, p);
	}

	// return the total reduction
	return dec;
}

// constructor for the "root" PathInfo
PathInfo::PathInfo(const AdjMat& costs) :
	infinite(costs.size * costs.size), bothBranches(true),
	lowerBound(0)
{
	infinite.size = costs.size;
	// set all elements on the diagonal as infinite
	for(unsigned int diag = 0; diag < costs.size; ++diag)
	{
		this->setInfinite(diag, diag);
	}
}

PathInfo::PathInfo(const PathInfo& old, Edge e, bool inc) :
	PathInfo(old)
{
	inc ? this->addInclude(e) : this->addExclude(e);
}

PathInfo::PathInfo() : bothBranches(true), lowerBound(infinity) {};

void PathInfo::addInclude(const Edge& e)
{
	// find the largest subtour involving the edge to add
	deque<unsigned int> subtour = { e.first, e.second };
	bool foundEdge = true;
	while(foundEdge)
	{
		foundEdge = false;
		for(const Edge& check : this->include)
		{
			// determine if "check" goes before in a subtour
			if(check.second == subtour.front())
			{
				subtour.push_front(check.second);
				subtour.push_front(check.first);
				foundEdge = true;
				break;
			}

			// determine if "check" goes after in a subtour
			else if(check.first == subtour.back())
			{
				subtour.push_back(check.first);
				subtour.push_back(check.second);
				foundEdge = true;
				break;
			}
		}
	}

	// add the edge onto include
	this->include.push_back(e);

	// make the ends of the longest subtour infinite
	this->setInfinite(subtour.back(), subtour.front());
}

void PathInfo::addExclude(const Edge& e)
{
	this->setInfinite(e.first, e.second);
}

void PathInfo::setAvailAndLB(const struct AdjMat& c, 
	struct MatrixInfo& info) 
{
	// reset lower bound
	this->lowerBound = 0;

	// start its calculation using the cost of the includes
	for(const Edge& e : this->include)
	{
		this->lowerBound += c(e.first, e.second);
		info.rowAvail[e.first] = false;
		info.colAvail[e.second] = false;
	}
}

// build the TSP path once it exists
// this method will infinite loop if there is not a full path
struct Path PathInfo::getTSPPath(const AdjMat& c) const
{
	// create the solution path
	Path solution;
	solution.vertices.reserve(this->include.size());
	solution.length = 0;

	// to help find the path
	unsigned int pastVertex = 0;
	unsigned int vertex = 0;

	// sort the edges and then find the path through them
	vector<Edge> edges = this->include;
	sort(edges.begin(), edges.end(), EdgeComp());
	for(unsigned int i = 0; i < edges.size(); ++i)
	{
		// push the next vertex on to the path
		solution.vertices.push_back(vertex);
		vertex = edges[vertex].second;

		// incremement the length of the path
		solution.length += c(pastVertex, vertex);
		pastVertex = vertex;
	}
	// finish the path, INVARIANT: vertex should always be 0
	solution.length += c(pastVertex, vertex);

	return solution;
}

ostream& operator<<(ostream& os, const PathInfo& p)
{
	os << "{ ";
	for(const Edge& e : p.include)
	{
		os << "(" << e.first << " " << e.second << ") ";
	}
	os << " } ";
	return os;
}

void PathInfo::calcLBAndNextEdge(const AdjMat& c) 
{
	// information about the useable matrix
	struct MatrixInfo info(c.size);

	// calculate the initial LB from edges already included
	// also, set any edges that would create a subtour as infinite
	this->setAvailAndLB(c, info);

	// reduce the matrix, increment the lower bound from the reduction
	this->lowerBound += info.reduceMatrix(c, *this);

	// find all the zeros in the matrix
	vector<Edge> zeros;
	for(unsigned int i = 0; i < c.size; ++i)
	{
		if(info.rowAvail[i])
		{
			for(unsigned int j = 0; j < c.size; ++j)
			{
				if(info.colAvail[j] && !this->isInfinite(i, j))
				{
					if(c(i, j) - info.rowReds[i] - 
						info.colReds[j] == 0)
					{
						zeros.push_back({ i, j });
					}
				}
			}
		}
	}
	
	
	// get a penalty associated with each zero
	vector<unsigned int> penalties(zeros.size());
	unsigned int counter = 0;
	for(const Edge zero : zeros)
	{
		bool foundMinCol = false;
		bool foundMinRow = false;
		unsigned int minCol = infinity;
		unsigned int minRow = infinity;

		// check for largest distance in row
		for(unsigned int i = 0; i < c.size; ++i)
		{
			// INVARIANT: column is always available
			if(info.rowAvail[i] && 
				!this->isInfinite(i, zero.second) && 
				i != zero.first && c(i, zero.second) - 
				info.rowReds[i] - info.colReds[zero.second] < minRow)
			{
				minRow = c(i, zero.second) - info.rowReds[i] - 
					info.colReds[zero.second];
				foundMinRow = true;
			}
		}

		// check largest distance in col
		for(unsigned int j = 0; j < c.size; ++j)
		{
			// INVARIANT: row is always available
			if(info.colAvail[j] && 
				!this->isInfinite(zero.first, j) && 
				zero.second != j && c(zero.first, j) - 
				info.rowReds[zero.first] - info.colReds[j] < minCol)
			{
				minCol = c(zero.first, j) - info.rowReds[zero.first] -
					info.colReds[j];
				foundMinCol = true;
			}
		}

		// 3 cases
		// 1. base case
		if(c.size - this->include.size() == 2)
		{
			penalties[counter++] = infinity;
			break;
		}

		// 2. case when excluding the node creates a disconnected graph
		else if(foundMinCol != foundMinRow && 
			c.size - this->include.size() != 2)
		{
			// we must choose this edge and cannot branch
			this->next = zero;
			this->bothBranches = false;
			return;
		}
		// 3. normal case, there is both an include and exclude branch
		else
		{
			penalties[counter++] = minRow + minCol;
		}
	}

	// set the next edge as the zero with the highest penalty
	auto maxPenaltyIt = max_element(penalties.begin(), penalties.end());
	unsigned int index = maxPenaltyIt - penalties.begin();

	// handle the base case
	if(c.size - this->include.size() == 2)
	{
		this->addInclude(zeros[index]);
		info.rowAvail[zeros[index].first] = false;
		info.colAvail[zeros[index].second] = false;
		unsigned int row = 0;
		unsigned int col = 0;

		// INVARIANT: only one valid edge left, find it
		for(unsigned int i = 0; i < info.rowAvail.size(); ++i)
		{
			if(info.rowAvail[i])
			{
				row = i;
				break;
			}
		}
		for(unsigned int j = 0; j < info.colAvail.size(); ++j)
		{
			if(info.colAvail[j])
			{
				col = j;
				break;
			}
		}
		this->addInclude({ row, col });
		// recalculate LB, the length of the TSP tour
		this->setAvailAndLB(c, info);
		throw NoNextEdge();
	}

	// for any other case, set the next edge to branch on
	// and set the flag that calculations have been completed
	this->next = zeros[index];
	this->foundLBAndEdge = true;
} 
