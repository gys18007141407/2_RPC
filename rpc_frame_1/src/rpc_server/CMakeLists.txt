add_compile_options(-std=c++14)

include_directories(${project_source_dir}/src/rpc_base)

set(SERVER_SRC rpc_server_test.cpp)

add_executable(rpc_server ${SERVER_SRC})