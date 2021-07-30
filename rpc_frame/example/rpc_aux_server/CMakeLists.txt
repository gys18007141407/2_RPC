add_compile_options(-std=c++11 -O0 -ggdb)

set(SERVER_FRIEND_CPP rpc_friend_server.cpp ../friend.pb.cc)

add_executable(rpc_friend_server ${SERVER_FRIEND_CPP})

target_link_libraries(rpc_friend_server simpleRPC protobuf)