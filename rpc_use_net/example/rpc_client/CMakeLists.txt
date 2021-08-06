add_compile_options(-std=c++14)

include_directories(${PROJECT_SOURCE_DIR}/src/rpc_base)
include_directories(${PROJECT_SOURCE_DIR}/src/rpc_client)
include_directories(${PROJECT_SOURCE_DIR}/extra/net/include)

link_directories(${PROJECT_SOURCE_DIR}/extra/net/lib)

aux_source_directory(. CPP)
add_executable(rpc_client ${CPP})

target_link_libraries(rpc_client rpcclient net log buffer timer pthread)