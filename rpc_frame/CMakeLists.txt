cmake_minimum_required(VERSION 3.0)
project(rpc_frame)

add_compile_options(-std=c++11 -O0 -ggdb)

# 生成debug版本能够调试
set(CMAKE_BUILD_TYPE "Debug")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/example)

link_libraries(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(src)
add_subdirectory(example)