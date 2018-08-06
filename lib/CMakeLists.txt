cmake_minimum_required (VERSION 2.8)
project (lib)
include_directories (BEFORE ${cmake_source_dir}/lib)
link_directories (${cmake_binary_dir}/lib)
add_library (lhf lhf.cpp)

install (TARGETS lhf
			RUNTIME DESTINATION bin
			ARCHIVE DESTINATION lib)
install (FILES lhf.h DESTINATION include)
