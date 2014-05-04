// INCLUDES
// STL
#include <unordered_set>
#include <algorithm>
// Project
#include "graph.hpp"

using std::vector;
using std::unordered_set;
using std::min_element;
using std::max_element;

// a struct for helping with the cheapest insertion heuristic
// keeps track of distances between a certain vertex and the rest
struct FastTable
{
	vector<unsigned int> distances;
	FastTable(unsigned int size) : distances(size) {};

	bool operator()(unsigned int first, unsigned int second)
	{
		return this->distances[first] < this->distances[second];
	}
};

struct Path Graph::fastTSP(bool useOpt)
{
	// catch the case with 0 vertices
	if(this->numVertices == 0)
	{
		return Path();
	}	

	// this is very similar to finding an MST
	FastTable t(this->numVertices);
	unordered_set<unsigned int> remaining;
	struct Path salespath;
	salespath.length = 0;

	// set the starting vertex's data
	unsigned int start = 0;
	t.distances[start] = 0;
	salespath.vertices.push_back(start);

	// set all the distances 
	for(unsigned int i = 1; i < this->numVertices; ++i)
	{
		t.distances[i] = this->operator()(start, i);
		remaining.insert(i);
	}

	// find the second vertex, the closest one to the beginning
	if(!remaining.empty())
	{
		// find the min distance to a vertex
		unsigned int minIndex = *min_element(remaining.cbegin(), 
			remaining.cend(), t);
		salespath.vertices.push_back(minIndex);
		remaining.erase(minIndex);
	}

	// while there are still unvisited vertices
	while(!remaining.empty())
	{
		// find the min distance to a vertex
		unsigned int minIndex = *min_element(remaining.cbegin(), 
			remaining.cend(), t);
		remaining.erase(minIndex);

		// find the cost of inserting between the first and second vertices
		// use that as a baseline
		unsigned int index1 = 0;
		unsigned int index2 = 1;
		unsigned int oldDistance = this->operator()(salespath.vertices[index1],
			salespath.vertices[index2]);
		unsigned int newDistance = this->operator()(salespath.vertices[index1],
			minIndex) + this->operator()(salespath.vertices[index2], minIndex);
		unsigned int minCost = newDistance - oldDistance;

		// find the two vertices that has the smallest cost of insertion
		for(unsigned int i = 1; i < salespath.vertices.size() - 1; ++i)
		{
			unsigned int oldDist = this->operator()(salespath.vertices[i],
				salespath.vertices[i + 1]);
			unsigned int newDist = this->operator()(salespath.vertices[i],
				minIndex) + this->operator()(salespath.vertices[i + 1], 
				minIndex);
			unsigned int cost = newDist - oldDist;
			if(cost < minCost)
			{
				minCost = cost;
				index1 = i;
				index2 = i + 1;
			}
		}

		// insert the vertex in the path
		auto pos = salespath.vertices.begin() + index2;
		salespath.vertices.insert(pos, minIndex);

		// update the distances of all entries
		for(unsigned int index : remaining)
		{
			unsigned int newDistance = this->operator()(minIndex, index);
			if(newDistance < t.distances[index])
			{
				t.distances[index] = newDistance;
			}
		}
	}

	// calculate and set the length of the path
	for(unsigned int i = 0; i < salespath.vertices.size(); ++i)
	{
		salespath.length += this->operator()(salespath.vertices[i], 
			salespath.vertices[(i + 1) % salespath.vertices.size()]);
	}

	return salespath;
}
