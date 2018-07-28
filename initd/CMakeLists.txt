cmake_minimum_required (VERSION 2.8)
project (initd)

message(STATUS "CMAKE_SOURCE_DIR:${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR}")
include_directories (BEFORE ${CMAKE_SOURCE_DIR}/lib/)
link_directories (${CMAKE_BINARY_DIR}/lib)
add_executable (daytimetcpsrv2 daytimetcpsrv2.cpp)
target_link_libraries (daytimetcpsrv2 lhf daemon_init)

install (TARGETS daytimetcpsrv2
		RUNTIME DESTINATION bin)
