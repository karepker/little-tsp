#include "graph/graph.hpp"

#include <iostream>

using std::ostream;

ostream& operator<<(ostream& os, const Graph& graph) {
	os << graph.Describe();
	return os;
}
