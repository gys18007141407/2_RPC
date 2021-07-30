add_compile_options(-std=c++11 -O0 -ggdb)
add_subdirectory(rpc_base_server)
add_subdirectory(rpc_client)

add_subdirectory(rpc_aux_server)