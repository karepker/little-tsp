#ifndef PATH_H
#define PATH_H

#include <iosfwd>
#include <vector>

// a struct for representing a path through a set of vertices
struct Path {
	Path() : length{0} {}

	bool operator==(const Path& other) const
	{ return vertices == other.vertices; }

	friend std::ostream& operator<<(std::ostream& os, const Path& p);

	// the order of vertices through which to travel
	std::vector<int> vertices;
	// the length of the path through that particular order
	int length;
};

#endif // PATH_H
