// INCLUDES
// Project
#include "basicgraph.hpp"

using std::ostream;
using std::endl;

ostream& operator<<(ostream& os, const struct Path& p)
{
	// print the size
	os << p.length << endl;

	// print the order of the nodes
	bool first = true;
	for(const unsigned int& node : p.vertices)
	{
		if(!first)
		{
			os << " ";
		}
		first = false;
		os << node;
	}
	os << endl;
	return os;
}

