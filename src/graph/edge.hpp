#ifndef EDGE_H
#define EDGE_H

#include <iosfwd>

// a simple edge structure
struct Edge {
	Edge() : u{-1}, v{-1} {}  // use invalid indices as default edge u and v
	Edge(int u_, int v_) : u{u_}, v{v_} {}
	Edge(const Edge&) = default;

	bool operator==(const Edge& other) const {
		return u == other.u && v == other.v;
	}
	bool operator!=(const Edge& other) const { return !(*this == other); }

	int u, v;
};

std::ostream& operator<<(std::ostream& os, const Edge& e);

#endif  // EDGE_H
