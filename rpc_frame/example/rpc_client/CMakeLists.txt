add_compile_options(-std=c++11 -O0 -ggdb)

set(CLIENT_CPP rpc_client.cpp ../user.pb.cc ../friend.pb.cc)

add_executable(rpc_client ${CLIENT_CPP})

target_link_libraries(rpc_client simpleRPC protobuf)