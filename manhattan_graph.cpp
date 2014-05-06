#include "manhattan_graph.hpp"

#include <string>
#include <sstream>

using std::cin;
using std::endl;
using std::istream;
using std::ostream;
using std::pair;
using std::string;
using std::stringstream
using std::vector;

ManhattanGraph::ManhattanGraph(istream& input)
{
	// some variables needed
	int line_num = 0;

	// read the map in
	while (true)
	{
		// if its the first line, get the number of vertices
		if (line_num == 0)
		{
			input >> world_size_;
			if (!input) { throw Error{"Couldn't read the world size"}; }
		}
		else if (line_num == 1)
		{
			input >> num_vertices_;
			if (!input) { throw Error{"Couldn't read number of vertices"}; }
		}
		// otherwise, get the coordinates
		else
		{
			int coord1, coord2;
			// first is x coordinate, second is y coordinate
			input >> coord1 >> coord2;
			// if we haven't reached the end of the file but can't read
			// coordinates, throw an Error
			if (!input || !input.eof()) { 
				throw Error{"Couldn't read coordinates!"}; 
			// if we've reached the end of the file, assume we've read all
			// vertices
			} else if (!input) { break; }
			vertices_.push_back({coord1, coord2});
		}
		line_num++;
	}
}

int ManhattanGraph::operator()(int from, int to) const {
	if (from < 0 || from > num_vertices_ || to < 0 || to > num_vertices_) {
		stringstream msg;
		msg << "Bad from or toumn provided: (" << from << ", " << to << 
			") limit is: " << num_vertices_ << endl;
		throw ImplementationError{msg.str().c_str()};
	}
	return abs(entries[from].x - entries[to].x) + 
		abs(entries[from].y - entries[to].y);
}

// TODO: not sure if this will stay
bool ManhattanGraph::is_valid_path(const struct Path& p) const
{
	// calculate the distance from each edge, make sure sum is correct
	unsigned int length = 0;
	for(unsigned int i = 0; i < p.vertices.size(); ++i)
	{
		length += this->operator()(p.vertices[i], 
			p.vertices[(i + 1) % p.vertices.size()]);
	}

	return length == p.length;
}

stringstream ManhattanGraph::Describe() const
{
	stringstream ss;

	// print information about the word
	ss << "Size of world: " << world_size_ << endl;
	ss << "Number of vertices: " << num_vertices_ << endl;

	// print the rows
	ss << "   ";
	for(int i = 0; i < num_vertices_; ++i) { ss << i << " "; }
	ss << endl;
	for(int row = 0; row < num_vertices_; ++row) {
		ss << " " << row << " ";
		for(int col = 0; col < num_vertices_; ++col) { 
			ss << operator()(row, col) << " "; 
		}
		ss << endl;
	}
	return ss;
}
