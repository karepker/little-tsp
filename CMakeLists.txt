cmake_minimum_required(VERSION 3.0.0)  # for CMAKE_CXX_COMPILER_ID
project (littletsp)

# Unit testing with Google Test
option(unit_tests "Build littletsp's unit tests" OFF)

message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# set some locations
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(GMOCK_ROOT "$ENV{HOME}/links/third_party/gmock-1.7.0")

find_package(gflags REQUIRED)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	if (CMAKE_COMPILER_IS_GNUCC)
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.8)
			message(FATAL_ERROR
				"Cannot use gcc version ${CMAKE_CXX_COMPILER_VERSION} < 4.8!")
		elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.9)
			set(CXXSTD "-std=c++11")
		else ()
			set(CXXSTD "-std=c++14")
		endif()
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
		# AppleClang versions mapped to vanilla clang versions using:
		# https://gist.github.com/yamaya/2924292
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.1)
			message(FATAL_ERROR "Cannot use AppleClang version \
				${CMAKE_CXX_COMPILER_VERSION} < 5.1!")
		elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 6.0)
			set(CXXSTD "-std=c++11")
		else ()
			set(CXXSTD "-std=c++14")
		endif()
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.3)
			message(FATAL_ERROR
				"Cannot use clang version ${CMAKE_CXX_COMPILER_VERSION} < 3.3!")
		elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.4)
			set(CXXSTD "-std=c++11")
		else ()
			set(CXXSTD "-std=c++14")
		endif()
	else ()
		message(FATAL_ERROR "Unknown clang dialect in use!")
	endif()


	set(COMMON_FLAGS "-Wall -Wextra -Werror -pedantic-errors ${CXXSTD}")
    set(CMAKE_CXX_FLAGS ${COMMON_FLAGS})
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
		# AppleClang versions mapped to vanilla clang versions using:
		# https://gist.github.com/yamaya/2924292
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.1)
			message(FATAL_ERROR "Cannot use AppleClang version \
				${CMAKE_CXX_COMPILER_VERSION} < 5.1!")
		elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 6.0)
			set(CXXSTD "-std=c++11")
		else ()
			set(CXXSTD "-std=c++14")
		endif()
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.3)
			message(FATAL_ERROR
				"Cannot use clang version ${CMAKE_CXX_COMPILER_VERSION} < 3.3!")
		elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.4)
			set(CXXSTD "-std=c++11")
		else ()
			set(CXXSTD "-std=c++14")
		endif()
	else ()
		message(FATAL_ERROR "Unknown clang dialect in use!")
	endif()

	set(COMMON_FLAGS "-Wall -Wextra -Werror	-pedantic-errors ${CXXSTD}")
    set(CMAKE_CXX_FLAGS ${COMMON_FLAGS})
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")
else ()
	message(FATAL_ERROR "Unsupported compiler! To add support, add a section \
	to CMakeLists.txt specifying flags for this compiler.")
endif()

# add directory for project source files
add_subdirectory(${SOURCE_DIR})
