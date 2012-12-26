#ifndef __BASICGRAPH__
#define __BASICGRAPH__

// INCLUDES
// STL
#include <vector>
#include <utility>
#include <iostream>

struct AdjMat
{
	// a vector of coordinates
	std::vector<std::pair<unsigned int, unsigned int> > entries;
	unsigned int size;

	// operators for getting costs
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

struct Path
{
	std::vector<unsigned int> vertices;
	unsigned int length;

	// operators
	bool operator==(const struct Path& other) const
	{ return this->length == other.length && 
		this->vertices == other.vertices; };

	friend std::ostream& operator<<(std::ostream& os,
		const struct Path& p);
};


#endif // __BASICGRAPH__
