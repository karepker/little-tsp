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
using std::ostream;

namespace {
	const unsigned int infinity = numeric_limits<unsigned int>::max();
}

// these methods will fail if there's no min
unsigned int CostMatrix::reduceRow(unsigned int i, const PathInfo& p,
	MatrixInfo& info) const
{
	unsigned int smallestElt = 0;

	// find the smallest element in the row
	if(info.rowAvail[i])
	{
		smallestElt = infinity;
		for(unsigned int j = 0; j < this->size(); ++j)
		{
			if(info.colAvail[j] && !p.isInfinite(i, j))
			{
				unsigned int current = this->entries(i, j) -
					info.rowReds[i] - info.colReds[j];
				if(current < smallestElt)
				{
					smallestElt = current;
				}
			}
		}
	}

	// reduce each element in the row by the smallest elt
	info.rowReds[i] += smallestElt;
	return smallestElt;
}

// these methods will fail if there's no min
unsigned int CostMatrix::reduceCol(unsigned int j, const PathInfo& p, 
	MatrixInfo& info) const
{
	unsigned int smallestElt = 0;

	// find the smallest element in the row
	if(info.colAvail[j])
	{
		smallestElt = infinity;
		for(unsigned int i = 0; i < this->size(); ++i)
		{
			if(info.rowAvail[i] && !p.isInfinite(i, j))
			{
				unsigned int current = this->entries(i, j) - 
					info.rowReds[i] - info.colReds[i];
				if(current < smallestElt)
				{
					smallestElt = current;
				}
			}
		}
	}

	// reduce each element in the row by the smallest elt
	info.colReds[j] +=  smallestElt;
	return smallestElt;
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

PathInfo::PathInfo(const PathInfo& old) :
	include(old.getInclude()), infinite(old.getInfinite()), 
	next(old.getNextEdge()), foundLBAndEdge(old.getFoundLBAndEdge()), 
	bothBranches(old.getBothBranches()), lowerBound(old.getLowerBound()) {}

PathInfo::PathInfo(const PathInfo& old, Edge e, bool inc) :
	include(old.getInclude()), infinite(old.getInfinite()),
	bothBranches(old.getBothBranches()), lowerBound(old.getLowerBound()) 
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

void CostMatrix::setAvailAndLB(PathInfo& p, struct MatrixInfo& info) const
{
	// reset lower bound
	p.resetLowerBound();
	// start its calculation using the cost of the includes
	for(const Edge e : p.getInclude())
	{
		p.incLowerBound(this->entries(e.first, e.second));
		info.rowAvail[e.first] = false;
		info.colAvail[e.second] = false;
	}
}

// build the TSP path once it exists
// this method will infinite loop if there is not a full path
struct Path CostMatrix::getTSPPath(const PathInfo& p) const
{
	Path solution;
	solution.vertices.reserve(p.getInclude().size());
	solution.length = 0;
	unsigned int vertex = 0;
	// look for the first vertex
	for(const Edge& e : p.getInclude())
	{
		if(e.first == vertex)
		{
			solution.vertices.push_back(e.first);
			vertex = e.second;
			break;
		}
	}

	// look for the rest of the path
	while(true)
	{
		for(const Edge& e : p.getInclude())
		{
			if(e.first == vertex)
			{
				solution.vertices.push_back(e.first);
				vertex = e.second;
				if(vertex == 0)
				{
					break;
				}
			}
		}
	}

	// calculate the length of the path
	for(unsigned int i = 0; i < solution.vertices.size(); ++i)
	{
		solution.length += this->entries(solution.vertices[i], 
			solution.vertices[(i + 1) % solution.vertices.size()]);
	}

	return solution;
}

ostream& operator<<(ostream& os, const PathInfo& p)
{
	os << "{ ";
	for(const Edge& e : p.getInclude())
	{
		os << "(" << e.first << " " << e.second << ") ";
	}
	os << " } ";
	return os;
}

void CostMatrix::calcLBAndNextEdge(PathInfo& p) const
{
	// information about the useable matrix
	struct MatrixInfo info(this->entries.size);

	// calculate the initial LB from edges already included
	// also, set any edges that would create a subtour as infinite
	this->setAvailAndLB(p, info);

	// reduce the matrix
	for(unsigned int i = 0; i < this->size(); ++i)
	{
		unsigned int dec = this->reduceRow(i, p, info);
		p.incLowerBound(dec);
	}

	for(unsigned int j = 0; j < this->size(); ++j)
	{
		unsigned int dec = this->reduceCol(j, p, info);
		p.incLowerBound(dec);
	}

	// find all the zeros in the matrix
	vector<Edge> zeros;
	for(unsigned int i = 0; i < this->size(); ++i)
	{
		if(info.rowAvail[i])
		{
			for(unsigned int j = 0; j < this->size(); ++j)
			{
				if(info.colAvail[j] && !p.isInfinite(i, j))
				{
					if(this->entries(i, j) - info.rowReds[i] - 
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
	for(Edge zero : zeros)
	{
		bool foundMinCol = false;
		bool foundMinRow = false;
		unsigned int minCol = infinity;
		unsigned int minRow = infinity;

		// check for largest distance in row
		for(unsigned int i = 0; i < this->size(); ++i)
		{
			// INVARIANT: column is always available
			if(info.rowAvail[i] && !p.isInfinite(i, zero.second) && 
				i != zero.first && this->entries(i, zero.second) - 
				info.rowReds[i] - info.colReds[zero.second] < minRow)
			{
				minRow = this->entries(i, zero.second) - info.rowReds[i] - 
					info.colReds[zero.second];
				foundMinRow = true;
			}
		}

		// check largest distance in col
		for(unsigned int j = 0; j < this->size(); ++j)
		{
			// INVARIANT: row is always available
			if(info.colAvail[j] && !p.isInfinite(zero.first, j) && 
				zero.second != j && this->entries(zero.first, j) - 
				info.rowReds[zero.first] - info.colReds[j] < minCol)
			{
				minCol = this->entries(zero.first, j) - info.rowReds[zero.first] - 
					info.colReds[j];
				foundMinCol = true;
			}
		}

		// base case
		if(this->size() - p.getInclude().size() == 2)
		{
			penalties[counter++] = infinity;
			break;
		}

		// when branching to another exclude creates a
		// disconnected graph
		else if(foundMinCol != foundMinRow && 
			this->size() - p.getInclude().size() != 2)
		{
			// we must choose this edge and cannot branch
			p.setNextEdge(zero);
			p.setBothBranches();
			return;
		}
		// normal case
		else
		{
			penalties[counter++] = minRow + minCol;
		}
	}

	// set the next edge as the zero with the highest penalty
	auto maxPenaltyIt = max_element(penalties.begin(), penalties.end());
	unsigned int index = maxPenaltyIt - penalties.begin();

	// base case: 2 x 2 matrix
	if(this->size() - p.getInclude().size() == 2)
	{
		p.addInclude(zeros[index]);
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
		p.addInclude({ row, col });
		// recalculate LB, the length of the TSP tour
		this->setAvailAndLB(p, info);
		throw NoNextEdge();
	}

	// for any other case, set the next edge to branch on
	// and set the flag that calculations have been completed
	p.setNextEdge(zeros[index]);
	p.setFoundLBAndEdge();
} 
