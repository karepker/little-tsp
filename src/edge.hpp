#ifndef EDGE_H
#define EDGE_H

#include <iosfwd>

// a simple edge structure
struct Edge {
	bool operator==(const Edge& other) const {
		return u == other.u && v == other.v; 
	}
	int u, v; 
};

std::ostream& operator<<(std::ostream& os, const Edge& e);

#endif  // EDGE_H
