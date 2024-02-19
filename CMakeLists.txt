message(STATUS "root")

cmake_minimum_required(VERSION 3.15)

project(
  engine
  LANGUAGES CXX
  VERSION 0.1.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(dependencies)
add_subdirectory(solado)
