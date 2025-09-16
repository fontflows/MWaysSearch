cmake_minimum_required(VERSION 3.29)
project(MWaysSearch)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

add_executable(MWaysSearch main.cpp MWayTree.cpp)

# Copy test files to build directory
configure_file(${CMAKE_SOURCE_DIR}/mvias.txt ${CMAKE_BINARY_DIR}/mvias.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/mvias2.txt ${CMAKE_BINARY_DIR}/mvias2.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/mvias3.txt ${CMAKE_BINARY_DIR}/mvias3.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/mvias4.txt ${CMAKE_BINARY_DIR}/mvias4.txt COPYONLY)