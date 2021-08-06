add_compile_options(-std=c++14)

include_directories(${PROJECT_SOURCE_DIR}/src/rpc_base)
include_directories(.)
include_directories(${PROJECT_SOURCE_DIR}/extra/net/include)


link_directories(${PROJECT_SOURCE_DIR}/extra/net/lib)

aux_source_directory(. CPP)

add_library(rpcserver ${CPP})


target_link_libraries(rpcserver net log timer buffer)