add_compile_options(-std=c++14)

include_directories(${project_source_dir}/src/rpc_base)
set(CLIENT_SRC rpc_client_test.cpp)

add_executable(rpc_client ${CLIENT_SRC})

target_link_libraries(rpc_client pthread)