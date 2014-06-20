cmake_minimum_required(VERSION 2.6)
project (littletsp)

# set some locations
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(UNITTEST_DIR "${SOURCE_DIR}/test")
set(GTEST_ROOT "$ENV{HOME}/links/thirdparty/gtest-1.7.0")

# set compilation variables
set(CXXFLAGS "-Wall -Werror -std=c++11")
set(CXXFLAGS_RELEASE "${CXXFLAGS} -O3")
set(CXXFLAGS_DEBUG "${CXXFLAGS} -g -O0")

# add directory for project source files
add_subdirectory(${SOURCE_DIR})

# Unit testing with Google Test
option(unit_tests "Build littletsp's unit tests" OFF)
if (unit_tests)
	if (NOT EXISTS "${GTEST_ROOT}")
		message("'${GTEST_ROOT}' does not exist, please set the GTEST_ROOT
		variable to the appropriate directory")
		return()
	endif()
	add_subdirectory(${UNITTEST_DIR})
	# add binaries of google test to the google test root
	add_subdirectory("${GTEST_ROOT}" "${GTEST_ROOT}")
endif()
