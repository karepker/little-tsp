#ifndef __PATHINFO__
#define __PATHINFO__

// INCLUDES
// STL
#include <vector>
#include <utility>
#include <iostream>
// Project
#include "matrix.hpp"
#include "basicgraph.hpp"

typedef std::pair<unsigned int, unsigned int> Edge;

// exception classes
class NoNextEdge {};

class PathInfo
{
	// edges that are being included and excluded
	std::vector<Edge> include;
	// bit matrix corresponding to the matrix cells set to infinite
	Matrix<bool> infinite; 

	// keeping track for future PathInfo's
	Edge next;
	bool foundLBAndEdge;
	bool bothBranches;

	// the node's lower bound
	unsigned int lowerBound;

public:

	// some constructors
	PathInfo();
	PathInfo(const AdjMat& costs);
	// copy constructor
	PathInfo(const PathInfo& old);
	PathInfo(const PathInfo& old, Edge e, bool inc);

	// Important methods
	// add included and excluded vertices
	void addInclude(const Edge& e);
	void addExclude(const Edge& e);

	// methods to deal with infinite
	const bool isInfinite(unsigned int i, unsigned int j) const
		{ return this->infinite(i, j); };
	bool isInfinite(unsigned int i, unsigned int j) 
		{ return this->infinite(i, j); };
	void setInfinite(unsigned int i, unsigned int j) 
		{ this->infinite.setEntry(i, j, true); }; 

	// functions called by getLBAndNextEdge
	void resetLowerBound() { this->lowerBound = 0; };
	void incLowerBound(unsigned int inc) { this->lowerBound += inc; };
	void setFoundLBAndEdge() { this->foundLBAndEdge = true; };
	void setNextEdge(Edge e) { this->next = e; };
	void setBothBranches() { this->bothBranches = false; };

	// getters
	const std::vector<Edge> getInclude() 
		const { return this->include; };
	const Matrix<bool> getInfinite() const { return this->infinite; };
	const unsigned int getLowerBound() const { return this->lowerBound; }; 
	unsigned int getSize() const { return this->infinite.size; };
	bool getFoundLBAndEdge() const { return this->foundLBAndEdge; };
	bool getBothBranches() const { return this->bothBranches; };
	Edge getNextEdge() const { return this->next; };
	
	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const PathInfo& p);
};


// CostMatrix doesn't changed, it is used to manipulate PathInfos
class CostMatrix
{
	const AdjMat entries;

	// reduce rows and columns
	// this will not modify the matrix, rather, the PathInfo
	unsigned int reduceRow(unsigned int i, const PathInfo& p, 
		MatrixInfo& info) const;
	unsigned int reduceCol(unsigned int j, const PathInfo& p, 
		MatrixInfo& info) const;

	void setAvailAndLB(PathInfo& p, struct MatrixInfo& info) const;

public:
	// get the next edge and the penalty for not choosing that edge
	// matrix must be reduced
	void calcLBAndNextEdge(PathInfo& p) const;
	CostMatrix(const AdjMat e) : entries(e) {};

	// getters
	unsigned int size() const { return this->entries.size; };

	// once there is a full TSP, get the path
	struct Path getTSPPath(const PathInfo& p) const;
};

#endif // __PATHINFO__
