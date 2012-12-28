#Little Traveling Salesperson Problem Implementation
An implementation of a branch and bound algorithm for the traveling salesperson problem as described by Little, Murty, Sweeney, and Karel in their 1963 paper

## Overview
Given a graph, this program finds the shortest Hamiltonian cycle starting and ending at vertex 0.

## Source
A pdf of the original paper can be found in this repository (`source.pdf`) as well as online [here](http://dspace.mit.edu/bitstream/handle/1721.1/46828/algorithmfortrav00litt.pdf) or [here](http://ia600502.us.archive.org/15/items/algorithmfortrav00litt/algorithmfortrav00litt.pdf). [This website](http://archive.org/details/algorithmfortrav00litt) gives many formats in which the source may be downloaded.

## Implementation
The implementation is given in C++. It stays relatively true to the description that the authors give for how to implement it to minimize computation and space complexity. The only addition is the use the "nearest insertion" heuristic (described [here](http://www.ida.liu.se/~TDDB19/reports_2003/htsp.pdf)), a basic heuristic for the TSP, to find an initial lower bound.

## Graph Interpretation
The graph is interpreted as a [complete graph](http://en.wikipedia.org/wiki/Complete_graph). That is, it is a simple undirected graph in which every pair of vertices is connected by a unique edge. The weight of each edge is the Manhattan (taxi cab) distance between the two vertices it connects.

## Input
The input file format is as follows. Line 1 contains the size `N` of the map. The map is considered an `NxN` square. The next line contains the number `M` of vertices. The next `M` lines contain the coordinate of each vertex `i` in the format `x y` where `x` is the x-coordinate of `i`, and `y` is the y-coordinate of `i`. There may be any number of empty lines at the end of the file. For example:

100  
4  
1 29  
2 82  
12 4  
3 76  

is a valid input file. The vertex with coordinates with coordinates `1 29` would be considered vertex 0. Input must be redirected to standard input `std::cin`. See the "Instructions to configure and run" for an example of how to run.

## Output
The shortest path found printed to standard output (`std::cout`) in the following format. Line 1 contains the total length of the cycle found. Line 2 contains each vertex `i` in the order that they must be visited in order to find the shortest cycle. This output always starts at vertex 0, and is assumed to finish at vertex 0 (though the final 0 is not printed). For example:

178  
0 1 3 2  

would be the output for the input file given above.

## Instructions to configure and run
After cloning the repository into a local directory, `cd` into that directory and type `./configure.sh; make`. To run individual cases of the program manually, type `littletsp < input.txt` (equivalently `littletsp -m OPTTSP < input.txt`), where `input.txt` is the path to a valid input file as described above. This program also provides a brute force algorithm for solving the TSP that conducts a simple depth first search and returns the minimum path. This may be used by typing `littletsp -m NAIVETSP < input.txt`. Also, typing `littletsp --help` will print a help message displaying options for running. Finally, running the demo script (type `./demo.sh`) will provide a demonstration of the Little algorithm's speedup vs the naive implementation.

## Modification
It would probably be most easy and useful to modify the input format and edges. To modify the input format, see the constructor for `Graph` in `src/graph.cpp`. To modify the way the weights for edges are calculated, or the way the adjacency matrix is stored, see `include/basicgraph.hpp` and `src/basicgraph.cpp`. To modify the initial heuristic used for an upper bound, look in `src/fasttsp.cpp`. The branch and bound algorithm itself can be found mostly in the `include/pathinfo.hpp`, `src/pathinfo.cpp`, and `src/opttsp.cpp` files. The commands `make debug` (builds `littletspd`, which has debugging flags) and `make clean` (cleans `.o` files, `littletsp`, and `littletspd` from the directory) may be useful for development purposes.

## License
little-tsp, an implementation of the branch and bound algorithm for the TSP as described in a 1963 paper by Little et al 
Copyright (C) 2012 Kar Epker

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
