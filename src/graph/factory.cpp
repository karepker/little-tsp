#include "graph/factory.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>

#include "graph/graph.hpp"
#include "graph/manhattan.hpp"
#include "util.hpp"

using std::cin;
using std::string;
using std::istream;
using std::unique_ptr;
using std::unordered_set;

const string manhattan_graph_type_c{"manhattan"};
const unordered_set<string> valid_graph_types{manhattan_graph_type_c};


bool IsValidGraphType(const string& type)
{ return valid_graph_types.find(type) != valid_graph_types.end(); }


unique_ptr<Graph> CreateGraph(const string& type, istream& input = cin) {
	if (type == manhattan_graph_type_c)
	{ return unique_ptr<Graph>{new ManhattanGraph{input}}; }
	throw Error{"Not a valid graph type!"};
}
