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
	tsp_solver/tsp_solver.cpp)

set(MAIN_SRC main.cpp)

set(UNITTEST_SRCS
	graph/edge_cost_test.cpp
	graph/mock.cpp
	tsp_solver/little/cost_matrix_test.cpp
	tsp_solver/little/tree_node_test.cpp)

set(DEBUG_OBJECT_LIBRARY "debug_objects")
set(DEBUG_BINARY "littletspd")
set(RELEASE_BINARY "littletsp")
set(UNITTEST_BINARY "unittest")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${gflags_INCLUDE_DIR})

add_library(${DEBUG_OBJECT_LIBRARY} OBJECT ${SRCS})
add_executable(${DEBUG_BINARY}
	${MAIN_SRC}
	$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)
add_executable(${RELEASE_BINARY} ${MAIN_SRC} ${SRCS})

set_property(SOURCE ${SRCS} PROPERTY COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${DEBUG_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${RELEASE_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_RELEASE}")
target_link_libraries(${DEBUG_BINARY}
	gflags
	pthread)
target_link_libraries(${RELEASE_BINARY}
	gflags
	pthread)

# if we choose to build unit tests, add rules for building unittest executable
if (unit_tests)
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
		$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)

	set_target_properties(${UNITTEST_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
	target_link_libraries(${UNITTEST_BINARY}
		gmock_main
		pthread)

endif()
