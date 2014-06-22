cmake_minimum_required(VERSION 2.8.8)  # for object libraries
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
