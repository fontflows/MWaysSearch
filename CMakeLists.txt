cmake_minimum_required(VERSION 3.29)
project(MWaysSearch)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

option(WARN_AS_ERROR "Tratar warnings como erro" ON)
if(WARN_AS_ERROR)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

option(ENABLE_SANITIZERS "Ativar sanitizers em Debug" OFF)
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address,undefined")
endif()

add_executable(MWaysSearch
        main.cpp
        MWayTree.cpp
        DataFile.cpp
)

configure_file(${CMAKE_SOURCE_DIR}/mvias.txt  ${CMAKE_BINARY_DIR}/mvias.txt  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/mvias2.txt ${CMAKE_BINARY_DIR}/mvias2.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/mvias3.txt ${CMAKE_BINARY_DIR}/mvias3.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/mvias4.txt ${CMAKE_BINARY_DIR}/mvias4.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/mvias5.txt ${CMAKE_BINARY_DIR}/mvias5.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/employees.txt ${CMAKE_BINARY_DIR}/employees.txt COPYONLY)
