#include "graph/edge.hpp"

#include <iostream>

using std::ostream;

ostream& operator<<(ostream& os, const Edge& e) {
	os << "(" << e.u << ", " << e.v << ")";
	return os;
}
