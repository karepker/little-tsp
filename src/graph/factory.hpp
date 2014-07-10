#ifndef GRAPH_FACTORY_H
#define GRAPH_FACTORY_H

#include <iosfwd>
#include <memory>
#include <string>

class Graph;

// construct a graph from input, by default, input assumed to be cin
std::unique_ptr<Graph> CreateGraph(const std::string& type,
		std::istream& input);

#endif  // GRAPH_FACTORY_H
