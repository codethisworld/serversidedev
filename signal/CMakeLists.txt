cmake_minimum_required (VERSION 2.8)
project (signal)

include_directories (BEFORE ${CMAKE_SOURCE_DIR}/lib)
link_directories (${CMAKE_BINARY_DIR}/lib)

add_executable (strsignal strsignal.cpp)

install(TARGETS strsignal
		RUNTIME DESTINATION bin)
