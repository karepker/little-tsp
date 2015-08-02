set(SRCS
	graph/edge.cpp
	graph/edge_cost.cpp
	graph/factory.cpp
	graph/graph.cpp
	graph/manhattan.cpp
	path.cpp
	tsp_solver/factory.cpp
	tsp_solver/fast.cpp
	tsp_solver/little/cost_matrix.cpp
	tsp_solver/little/solver.cpp
	tsp_solver/little/tree_node.cpp
	tsp_solver/naive.cpp
	tsp_solver/tsp_solver.cpp
	)

set(MAIN_SRC main.cpp)

set(UNITTEST_SRCS
	graph/edge_cost_test.cpp
	graph/mock.cpp
	tsp_solver/little/cost_matrix_test.cpp
	tsp_solver/little/tree_node_test.cpp
	)

set(OBJECT_LIBRARY "littletsp_objects")
set(BINARY "littletsp")
set(UNITTEST_BINARY "unittest")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${gflags_INCLUDE_DIR})

# remove "unused variable" warning for the main module for gflags
set_source_files_properties(${MAIN_SRC} PROPERTIES
	COMPILE_FLAGS -Wno-unused-variable
	)

add_library(${OBJECT_LIBRARY} OBJECT ${SRCS})
add_executable(${BINARY}
	${MAIN_SRC}
	$<TARGET_OBJECTS:${OBJECT_LIBRARY}>
	)

target_link_libraries(${BINARY}
	gflags
	pthread
	)

# if we choose to build unit tests, add rules for building unittest executable
if (unit_tests)
    message(STATUS "Unit test target available.")

	if (NOT EXISTS "${GMOCK_ROOT}")
		message("'${GMOCK_ROOT}' does not exist, please set the GMOCK_ROOT
		variable to the appropriate directory")
		return()
	endif()

	# add binaries of google test and google mock to their their roots
	add_subdirectory("${GMOCK_ROOT}" "${GMOCK_ROOT}")

	include_directories(
		${GMOCK_ROOT}/gtest
		${GMOCK_ROOT}/gtest/include
		${GMOCK_ROOT}
		${GMOCK_ROOT}/include)

	add_executable(${UNITTEST_BINARY}
		${UNITTEST_SRCS}
		$<TARGET_OBJECTS:${OBJECT_LIBRARY}>)

	target_link_libraries(${UNITTEST_BINARY}
		gmock_main
		pthread)

endif()
