cmake_minimum_required (VERSION 2.8)
project (names)

message(STATUS "CMAKE_SOURCE_DIR:${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR}")
include_directories (BEFORE ${CMAKE_SOURCE_DIR}/lib/)
link_directories (${CMAKE_BINARY_DIR}/lib)
add_executable (hostent hostent.cpp)
add_executable (daytimetcpcli1 daytimetcpcli1.cpp)
target_link_libraries (hostent lhf)
target_link_libraries (daytimetcpcli1 lhf)

install (TARGETS hostent daytimetcpcli1
			RUNTIME DESTINATION bin)
