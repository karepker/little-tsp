set(UNITTEST_BINARY "test")

include_directories(
	${SOURCE_DIR}	
	${GTEST_ROOT}/include
	${GTEST_ROOT})

add_executable(${UNITTEST_BINARY}
	simple.cpp)

target_link_libraries(${UNITTEST_BINARY}
	gtest_main
	pthread)

set_target_properties(${UNITTEST_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
