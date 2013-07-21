#ifndef __BASICGRAPH__
#define __BASICGRAPH__

// INCLUDES
// STL
#include <vector>
#include <utility>
#include <iostream>

// A simulation of an adjacency matrix for Manhattan distances
// coordinates are actually just stored in a vector
// this is because distances are symmetric and can be computed on the fly
struct AdjMat
{
	// a vector of coordinates
	std::vector<std::pair<unsigned int, unsigned int> > entries;
	unsigned int size;

	// operators for getting costs
	// note: "row" and "column" are just for familiarity
	// distances are computed on the fly, so they just represent indices
	const unsigned int operator()(unsigned int row, 
		unsigned int col) const
	{ 
		return abs(entries[row].first - entries[col].first) + 
				abs(entries[row].second - entries[col].second);
	}
	unsigned int operator()(unsigned int row, unsigned int col)
	{ 
		return abs(entries[row].first - entries[col].first) + 
				abs(entries[row].second - entries[col].second);
	}
	
	// constructors
	AdjMat(std::vector<std::pair<unsigned int, unsigned int> > e) :
		entries(e), size(entries.size()) {};

	AdjMat() : size(0) {};
};

// a struct for representing a path through a set of vertices
struct Path
{
	// the order of vertices through which to travel
	std::vector<unsigned int> vertices;
	// the length of the path through that particular order
	unsigned int length;

	// operators
	bool operator==(const struct Path& other) const
	{ return this->length == other.length && 
		this->vertices == other.vertices; };

	friend std::ostream& operator<<(std::ostream& os,
		const struct Path& p);
};


#endif // __BASICGRAPH__
