cmake_minimum_required(VERSION 3.0)
project(slicer)

if (UNIX)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif (UNIX)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(slicer ${SOURCES})