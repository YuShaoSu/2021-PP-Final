cmake_minimum_required(VERSION 2.8)
project( OpenMP )
find_package( OpenCV REQUIRED )
add_executable( OpenMP openmp.cpp image.cpp filter.cpp)
target_link_libraries( OpenMP ${OpenCV_LIBS} )
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()