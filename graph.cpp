// INCLUDES
// STL
#include <string>
// Projects
#include "graph.hpp"

using std::vector;
using std::pair;
using std::string;
using std::cin;
using std::endl;
using std::ostream;

Graph::Graph(std::istream& input)
{
	// some variables needed
	unsigned int lineNum = 0;
	unsigned int worldSize = 0;
	unsigned int numCakes = 0;
	vector<pair<unsigned int, unsigned int> > vertices;	
	int coord1 = -1;
	int coord2 = -1;

	// read the map in
	while(lineNum < numCakes + 2 || lineNum < 2)
	{
		// if its the first line, get the number of vertices
		if(lineNum == 0)
		{
			input >> worldSize;
			this->worldSize = worldSize;
		}
		else if(lineNum == 1)
		{
			input >> numCakes;
			this->numVertices = numCakes;
			vertices.reserve(numCakes);
		}
		// otherwise, get the coordinates
		else
		{
			input >> coord1 >> coord2;
			// first is x coordinate, second is y coordinate
			vertices.push_back({ coord1, coord2 });
		}
		lineNum++;
	}
	// build the adjacency "matrix" (really just a vector)
	this->adjMat = AdjMat(vertices);
}

// for testing
bool Graph::isValidPath(const struct Path& p) const
{
	// calculate the distance from each edge, make sure sum is correct
	unsigned int length = 0;
	for(unsigned int i = 0; i < p.vertices.size(); ++i)
	{
		length += this->operator()(p.vertices[i], 
			p.vertices[(i + 1) % p.vertices.size()]);
	}

	return length == p.length;
}

ostream& operator<<(ostream& os, const Graph& g)
{
	// print information about the word
	os << "Size of world: " << g.worldSize << endl;
	os << "Number of vertices: " << g.numVertices << endl;

	// print the rows
	os << "   ";
	for(unsigned int i = 0; i < g.numVertices; ++i)
	{
		os << i << " ";
	}
	os << endl;
	for(unsigned int row = 0; row < g.numVertices; ++row)
	{
		os << " " << row << " ";
		for(unsigned int col = 0; col < g.numVertices; ++col)
		{
			os << g(row, col) << " ";
		}
		os << endl;
	}
	return os;
}
	
