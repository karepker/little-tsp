# Little Traveling Salesperson Problem Implementation
An implementation of a branch and bound algorithm for the traveling salesperson problem as described by Little, Murty, Sweeney, and Karel in their 1963 paper


## Overview
Given a graph with weights between edges integers, this program finds the shortest Hamiltonian cycle starting and ending at vertex 0.


## Source
A pdf of the original paper can be found in this repository (`source.pdf`) as well as online [here](http://dspace.mit.edu/bitstream/handle/1721.1/46828/algorithmfortrav00litt.pdf) or [here](http://ia600502.us.archive.org/15/items/algorithmfortrav00litt/algorithmfortrav00litt.pdf). [This website](http://archive.org/details/algorithmfortrav00litt) gives many formats in which the source may be downloaded. When implementing the algorithm, I found [this video](https://www.youtube.com/watch?v=nN4K8xA8ShM) helped me understand it better.


## Implementation
The implementation is given in C++. It stays relatively true to the description that the authors give for how to implement it to minimize computation and space complexity.


## Instructions to configure and run
Dependencies:
* [cmake](http://www.cmake.org/). Should be available in your package manager on Linux.
* [gflags](https://github.com/gflags/gflags). Available as `gflags` for Debian-based distributions, otherwise, clone the repo (instructions given in the URL), create a `build/` subdirectory, run `cmake .. && sudo make install` to install on your system.
* (Optional) [Google Test](https://github.com/google/googletest) (for unit testing). Cloning repo and modifying the `GMOCK_ROOT` variable in the base `CMakeLists.txt` file should be sufficient for CMake to find it.

After cloning the repository into a local directory, `cd` into that directory and run `./configure`, then `cd` into `build/` and run `make`. To run individual cases of the program manually, run `./build/src/littletsp < input.txt` from the top level of the directory where `input.txt` is the path to a valid input file as described below.

This program also provides a brute force algorithm for solving the TSP that conducts a simple depth first search and returns the minimum path (provide `--solver=naive` on the command line). A heuristic-based approach is available too (`--solver=fast`). Also, typing `littletsp --help` will print a help message displaying options for running.

Finally, running the demo script (`python3 test/demo.py`) will provide a demonstration of Little's algorithm's speedup vs the naive implementation. See options for this script by typing `python3 test/demo.py --help`.

To build the unit tests, download Google Mock anywhere on your system. Then, open `CMakeLists.txt` and change the `GMOCK_ROOT` variable to the location where you downloaded it on your system. Then, build the unittest binary by running `make unittest` from `build/` and run the unittest binary with `./src/unittest`


## Input
The input file format is as follows. Line 1 contains the size `N` of the map. The map is considered an `NxN` square. The next line contains the number `M` of vertices. The next `M` lines contain the coordinate of each vertex `i` in the format `x y` where `x` is the x-coordinate of `i`, and `y` is the y-coordinate of `i`. There may be any number of empty lines at the end of the file. For example:

100  
4  
1 29  
2 82  
12 4  
3 76  

is a valid input file. The vertex with coordinates with coordinates `1 29` would be considered vertex 0. Input must be redirected to standard input `std::cin`. See the "Instructions to configure and run" for an example of how to run.

The graph is interpreted as a [complete graph](http://en.wikipedia.org/wiki/Complete_graph). That is, it is a simple undirected graph in which every pair of vertices is connected by a unique edge. The weight of each edge is the Manhattan (taxi cab) distance between the two vertices it connects.


## Output
The shortest path found printed to standard output (`std::cout`) in the following format. Line 1 contains the total length of the cycle found. Line 2 contains each vertex `i` in the order that they must be visited in order to find the shortest cycle. This output always starts at vertex 0, and is assumed to finish at vertex 0 (though the final 0 is not printed). For example:

178  
0 1 3 2  

would be the output for the input file given above.


## Organization
All source code can be found in `src/`.

Top level modules (`main`, `path`, `util`) contain code useful for running the entire binary.

The `src/graph/` subdirectory contains an object-oriented implementation of a Graph. See `graph/graph` for the interface, and `graph/manhattan` for an implementation of this interface. `graph/factory` is used as a factory for constructing graph objects.

The `src/tsp_solver/` subdirectory contains an object-oriented implementation of various solvers for the TSP problem. `src/tsp_solver/naive` gives the naive implementation, and `src/tsp_solver/fast` the fast heuristic-based approach. `src/tsp_solver/tsp_solver` gives the interface solvers must implement, and `src/tsp_solver/factory` provides a factory for constructing various solvers. The implementation of Little's algorithm can be found in `src/tsp_solver/little/` (it had enough source files to deserve its own implementation).

In `src/tsp_solver/little/solver` `LittleTSPSolver` provides the top-level operations for solving the TSP with Little's algorithm. It creates `TreeNode`s, which then create `CostMatrix`s, which are used to find the next edge to branch on. `LittleTSPSolver` creates these branches (at most 2, an branch including that edge, and a branch exluding that edge), and branches toward the include branch. When a valid solution is found, it is stored and used to prune branches which have a higher lower bound than the cost of the solution.


## License
little-tsp, an implementation of the branch and bound algorithm for the TSP as described in a 1963 paper by Little et al
Copyright (C) 2012-2015 Kar Epker

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
