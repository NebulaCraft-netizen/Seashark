cmake_minimum_required(VERSION 3.10)
project(Seashark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(${INCLUDE_DIR})
file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)

add_executable(seashark ${SOURCES})

