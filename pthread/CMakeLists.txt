cmake_minimum_required (VERSION 2.8)
project (pthread)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11" )
include_directories (BEFORE ${CMAKE_SOURCE_DIR}/lib)
link_directories (${CMAKE_BINARY_DIR}/lib)

add_executable (concurrent_array concurrent_array.cpp)

target_link_libraries (concurrent_array lhf pthread)

install(TARGETS concurrent_array
		RUNTIME DESTINATION bin)
