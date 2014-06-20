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

set(DEBUG_BINARY "littletspd")
set(RELEASE_BINARY "littletsp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${DEBUG_BINARY} ${SRCS})
add_executable(${RELEASE_BINARY} ${SRCS})

set_target_properties(${DEBUG_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${RELEASE_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_RELEASE}")
