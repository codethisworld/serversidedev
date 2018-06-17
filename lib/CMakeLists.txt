cmake_minimum_required (VERSION 2.8)
project (lib)
add_library (lhf lhf.cpp)
install (TARGETS lhf
			ARCHIVE DESTINATION lib)
install (FILES lhf.h DESTINATION include)
