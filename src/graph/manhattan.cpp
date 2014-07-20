#include "graph/manhattan.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "graph/edge.hpp"
#include "graph/edge_cost.hpp"
#include "util.hpp"

using std::cin;
using std::endl;
using std::istream;
using std::ostream;
using std::string;
using std::stringstream;
using std::vector;

ManhattanGraph::ManhattanGraph(istream& input = cin) {
	int line_num{0};
	vector<Coordinate> vertices;

	// read the map in
	while (true) {
		// if its the first line, get the number of vertices
		if (line_num == 0) {
			input >> world_size_;
			if (!input) { throw Error{"Couldn't read the world size"}; }
		} else if (line_num == 1) {
			input >> num_vertices_;
			edges_.SetSize(num_vertices_);
			if (!input) { throw Error{"Couldn't read number of vertices"}; }
		} else {
		// otherwise, get the coordinates
			int coord1, coord2;
			// first is x coordinate, second is y coordinate
			input >> coord1 >> coord2;
			// if we haven't reached the end of the file but can't read
			// coordinates, throw an Error
			if (!input && !input.eof()) {
				throw Error{"Couldn't read coordinates!"};
			// if we've reached the end of the file, assume we've read all
			// vertices
			} else if (!input) { break; }
			vertices.push_back(Coordinate{coord1, coord2});
		}
		line_num++;
	}

	// make and store EdgeCosts
	for (int row{0}; row < GetNumVertices(); ++row) {
		for (int column{0}; column < GetNumVertices(); ++column) {
			edges_(row, column) = EdgeCost{
				abs(vertices[row].x - vertices[column].x) +
					abs(vertices[row].y - vertices[column].y),
					Edge{row, column}};
		}
	}
}

const EdgeCost& ManhattanGraph::operator()(int from, int to) const {
	ValidateEdge(from, to);
	return edges_(from, to);
}

EdgeCost& ManhattanGraph::operator()(int from, int to) {
	ValidateEdge(from, to);
	return edges_(from, to);
}

const EdgeCost& ManhattanGraph::operator()(const Edge& e) const {
	ValidateEdge(e.u, e.v);
	return edges_(e.u, e.v);
}

EdgeCost& ManhattanGraph::operator()(const Edge& e) {
	ValidateEdge(e.u, e.v);
	return edges_(e.u, e.v);
}

string ManhattanGraph::Describe() const {
	stringstream ss;

	// print information about the word
	ss << "Size of world: " << world_size_ << endl;
	ss << "Number of vertices: " << GetNumVertices() << endl;

	// print the rows
	ss << "   ";
	for (int i = 0; i < GetNumVertices(); ++i) { ss << i << " "; }
	ss << endl;
	for (int row = 0; row < GetNumVertices(); ++row) {
		ss << " " << row << " ";
		for (int col = 0; col < GetNumVertices(); ++col) {
			ss << operator()(row, col)() << " ";
		}
		ss << endl;
	}
	return ss.str();
}

void ManhattanGraph::ValidateEdge(int from, int to) const {
	if (from < 0 || from > num_vertices_ || to < 0 || to > num_vertices_) {
		stringstream msg;
		msg << "Bad from or to provided: (" << from << ", " << to <<
			") limit is: " << num_vertices_ << endl;
		throw ImplementationError{msg.str().c_str()};
	}
}
