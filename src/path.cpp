#include "path.hpp"

#include <iostream>

using std::ostream;
using std::endl;

ostream& operator<<(ostream& os, const Path& p)
{
	// print the size
	os << p.length << endl;

	// print the order of the nodes
	bool first = true;
	for (const auto node : p.vertices)
	{
		if (!first) { os << " "; }
		first = false;
		os << node;
	}
	return os;
}

