add_compile_options(-std=c++14)

include_directories(${PROJECT_SOURCE_DIR}/src/rpc_base)
include_directories(${PROJECT_SOURCE_DIR}/src/rpc_server)
include_directories(${PROJECT_SOURCE_DIR}/extra/net/include)

link_directories(${PROJECT_SOURCE_DIR}/extra/net/lib)

aux_source_directory(. CPP)

add_executable(rpc_server ${CPP})

target_link_libraries(rpc_server rpcserver net log buffer timer pthread)