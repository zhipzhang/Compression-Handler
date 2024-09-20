cmake_minimum_required(VERSION 3.14)
project(CompressionHandler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


enable_testing()
include_directories(include)
add_subdirectory(src)
add_subdirectory(test)
