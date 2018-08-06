cmake_minimum_required (VERSION 2.8)
project (io)

include_directories (BEFORE ${CMAKE_SOURCE_DIR}/lib)
link_directories (${CMAKE_BINARY_DIR}/lib)

add_executable (css_echo_poll css_echo_poll.cpp)
add_executable (csc_echo_poll csc_echo_poll.cpp)
add_executable (css_echo_select css_echo_select.cpp)
add_executable (csc_echo_select csc_echo_select.cpp)
add_executable (css_echo_epoll css_echo_epoll.cpp)
add_executable (csc_echo_epoll csc_echo_epoll.cpp)

add_library (socketio socketio.cpp)

target_link_libraries (csc_echo_poll lhf socketio)
target_link_libraries (css_echo_poll lhf socketio)
target_link_libraries (css_echo_select lhf socketio)
target_link_libraries (csc_echo_select lhf socketio)
target_link_libraries (css_echo_epoll lhf socketio)
target_link_libraries (csc_echo_epoll lhf socketio pthread)

install(TARGETS csc_echo_poll css_echo_poll css_echo_select csc_echo_select css_echo_epoll csc_echo_epoll socketio
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install (FILES socketio.h DESTINATION include)
