#include "tsp_solver/fast.hpp"

#include <unordered_set>
#include <algorithm>

#include "graph/graph.hpp"
#include "graph/edge_cost.hpp"
#include "path.hpp"

using std::vector;
using std::unordered_set;
using std::min_element;
using std::max_element;

// a struct for helping with the cheapest insertion heuristic
// keeps track of distances between a certain vertex and the rest
struct FastTable
{
	vector<int> distances;
	FastTable(int size) : distances{size} {}

	bool operator()(int first, int second) {
		return distances[first] < distances[second];
	}
};

Path FastTSPSolver::Solve(const Graph& graph) const {
	// catch the case with 0 vertices
	if (graph.GetNumVertices() == 0) { return Path{}; }

	// this is very similar to finding an MST
	FastTable t{graph.GetNumVertices()};
	unordered_set<int> remaining;
	Path salespath;

	// set the starting vertex's data
	int start = 0;
	t.distances[start] = 0;
	salespath.vertices.push_back(start);

	// set all the distances 
	for (int i = 1; i < graph.GetNumVertices(); ++i)
	{
		t.distances[i] = graph(start, i)();
		remaining.insert(i);
	}

	// fi%s//d the second vertex, the closest one to the beginning
	if (!remaining.empty())
	{
		// find the min distance to a vertex
		int minIndex = *min_element(remaining.cbegin(),
			remaining.cend(), t);
		salespath.vertices.push_back(minIndex);
		remaining.erase(minIndex);
	}

	// while there are still unvisited vertices
	while (!remaining.empty())
	{
		// find the min distance to a vertex
		int minIndex = *min_element(remaining.cbegin(),
			remaining.cend(), t);
		remaining.erase(minIndex);

		// find the cost of inserting between the first and second vertices
		// use that as a baseline
		int index1 = 0;
		int index2 = 1;
		int oldDistance = graph(salespath.vertices[index1],
			salespath.vertices[index2])();
		int newDistance = graph(salespath.vertices[index1],
			minIndex)() + graph(salespath.vertices[index2], minIndex)();
		int minCost = newDistance - oldDistance;

		// find the two vertices that has the smallest cost of insertion
		for (int i = 1; i < int(salespath.vertices.size()) - 1; ++i)
		{
			int oldDist = graph(salespath.vertices[i],
				salespath.vertices[i + 1])();
			int newDist = graph(salespath.vertices[i],
				minIndex)() + graph(salespath.vertices[i + 1],
				minIndex)();
			int cost = newDist - oldDist;
			if (cost < minCost)
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
		for (int index : remaining)
		{
			int newDistance = graph(minIndex, index)();
			if (newDistance < t.distances[index])
			{
				t.distances[index] = newDistance;
			}
		}
	}

	// calculate and set the length of the path
	for (int i = 0; i < int(salespath.vertices.size()); ++i)
	{
		salespath.length += graph(salespath.vertices[i],
			salespath.vertices[(i + 1) % salespath.vertices.size()])();
	}

	return salespath;
}
