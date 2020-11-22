cmake_minimum_required(VERSION 3.0)
project(slicer)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(slicer ${SOURCES})