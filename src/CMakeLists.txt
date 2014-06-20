set(SRCS 
	graph/factory.cpp
	graph/graph.cpp
	graph/manhattan.cpp
	interaction.cpp
	main.cpp
	path.cpp
	tsp_solver/factory.cpp
	tsp_solver/fast_tsp_solver.cpp
	tsp_solver/little_tsp/cost_matrix.cpp
	tsp_solver/little_tsp/cost_matrix_integer.cpp
	tsp_solver/little_tsp/solver.cpp
	tsp_solver/little_tsp/tree_node.cpp
	tsp_solver/naive_tsp_solver.cpp
	tsp_solver/tsp_solver.cpp)

set(UNITTEST_SRCS
	tsp_solver/little_tsp/cost_matrix_integer_test.cpp)

set(DEBUG_BINARY "littletspd")
set(RELEASE_BINARY "littletsp")
set(UNITTEST_BINARY "test")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${DEBUG_BINARY} ${SRCS})
add_executable(${RELEASE_BINARY} ${SRCS})

set_target_properties(${DEBUG_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${RELEASE_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_RELEASE}")

if (unit_tests)
	if (NOT EXISTS "${GTEST_ROOT}")
		message("'${GTEST_ROOT}' does not exist, please set the GTEST_ROOT
		variable to the appropriate directory")
		return()
	endif()
	# add binaries of google test to the google test root
	add_subdirectory("${GTEST_ROOT}" "${GTEST_ROOT}")

	include_directories(
		${GTEST_ROOT}/include
		${GTEST_ROOT})

	add_executable(${UNITTEST_BINARY} ${UNITTEST_SRCS})

	set_target_properties(${UNITTEST_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
	target_link_libraries(${UNITTEST_BINARY} 
		gtest_main
		pthread)

endif()

