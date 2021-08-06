
cmake_minimum_required(VERSION 3.0)

project(rpc)

add_compile_options(-std=c++14)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


add_subdirectory(src)
add_subdirectory(example)
