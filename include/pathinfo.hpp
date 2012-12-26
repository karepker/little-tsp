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

	// getters
	const std::vector<Edge> getInclude() 
		const { return this->include; };
	const Matrix<bool> getInfinite() const { return this->infinite; };
	const unsigned int getLowerBound() const 
		{ return this->lowerBound; }; 
	bool getFoundLBAndEdge() const { return this->foundLBAndEdge; };

	// branching methods
	bool getBothBranches() const { return this->bothBranches; };
	Edge getNextEdge() const { return this->next; };
	
	// branch determination methods
	void setAvailAndLB(const struct AdjMat& c, 
		struct MatrixInfo& info);
	void calcLBAndNextEdge(const struct AdjMat& c);
	struct Path getTSPPath(const AdjMat& c) const;
	
	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const PathInfo& p);
};

// information about the useable matrix
// temporary struct that is used to help build a PathInfo
struct MatrixInfo
{
	// whether the rows are available
	std::vector<unsigned int> rowReds;
	std::vector<unsigned int> colReds;

	// whether the columns are available
	std::vector<bool> rowAvail;
	std::vector<bool> colAvail;

	MatrixInfo(unsigned int size) : rowReds(size, 0), colReds(size, 0),
		rowAvail(size, true), colAvail(size, true) {};

	// reduce the matrix by storing information about reduction
	unsigned int reduceRow(unsigned int i, 
		const struct AdjMat& c,	const struct PathInfo& p);
	unsigned int reduceCol(unsigned int j, 
		const struct AdjMat& c, const struct PathInfo& p);
	unsigned int reduceMatrix(const struct AdjMat& c,
		const struct PathInfo& p);
};

#endif // __PATHINFO__
