cmake_minimum_required (VERSION 2.8)
project (algo)

include_directories (BEFORE "${CMAKE_SOURCE_DIR}/lib")
link_directories ("${CMAKE_BINARY_DIR}/lib")

add_executable (bit_reverse bit_reverse.cpp)

install (TARGETS bit_reverse
			RUNTIME DESTINATION bin)
