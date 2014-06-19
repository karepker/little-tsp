include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CXXFLAGS "-Wall -Werror -std=c++11")
set(CXXFLAGS_RELEASE "${CXXFLAGS} -O3")
set(CXXFLAGS_DEBUG "${CXXFLAGS} -g -O0")

set(SRCS 
	fast_tsp_solver.cpp
	graph.cpp
	graph_factory.cpp
	interaction.cpp
	tsp_solver.cpp
	tsp_solver_factory.cpp
	little_tsp_cost_matrix.cpp
	little_tsp_cost_matrix_integer.cpp
	little_tsp_solver.cpp
	little_tsp_tree_node.cpp
	main.cpp
	manhattan_graph.cpp
	naive_tsp_solver.cpp
	path.cpp)

add_executable(littletspd ${SRCS})
add_executable(littletsp ${SRCS})

set_target_properties(littletspd PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(littletsp PROPERTIES COMPILE_FLAGS "${CXXFLAGS_RELEASE}")
