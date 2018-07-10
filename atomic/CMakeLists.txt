cmake_minimum_required (VERSION 2.8)
project (atomic)

include_directories (BEFORE ${CMAKE_SOURCE_DIR}/lib)
link_directories (${CMAKE_BINARY_DIR}/lib)

add_executable(HAVE_IB_GCC_ATOMIC_BUILTINS HAVE_IB_GCC_ATOMIC_BUILTINS.cpp)
add_executable(test_gcc_atomic test_gcc_atomic.cpp)

target_link_libraries (test_gcc_atomic lhf pthread)

install(TARGETS HAVE_IB_GCC_ATOMIC_BUILTINS test_gcc_atomic
		RUNTIME DESTINATION bin)
