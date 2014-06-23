#include "graph/factory.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "graph/graph.hpp"
#include "graph/manhattan.hpp"
#include "util.hpp"

using std::cin;
using std::string;
using std::istream;
using std::unique_ptr;

const string manhattan_graph_type_c{"manhattan"};

unique_ptr<Graph> CreateGraph(const string& type, istream& input = cin) {
	if (type == manhattan_graph_type_c) 
	{ return unique_ptr<Graph>{new ManhattanGraph{input}}; }
	throw Error{"Not a valid graph type!"};
}
