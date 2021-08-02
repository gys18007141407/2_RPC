
cmake_minimum_required(VERSION 3.0)

project(rpc)

add_compile_options(-std=c++14)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src)