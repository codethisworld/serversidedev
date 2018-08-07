cmake_minimum_required (VERSION 2.8)
project (basic)

include_directories (BEFORE ${CMAKE_SOURCE_DIR}/lib)
link_directories (${CMAKE_BINARY_DIR}/lib)

#add_executable(HAVE_IB_GCC_ATOMIC_BUILTINS HAVE_IB_GCC_ATOMIC_BUILTINS.cpp)
add_executable(qsort qsort.cpp)
add_executable(test_free_null test_free_null.c)

#target_link_libraries (test_gcc_atomic lhf pthread)

install(TARGETS qsort test_free_null
		RUNTIME DESTINATION bin
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib)
