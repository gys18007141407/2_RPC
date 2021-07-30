add_compile_options(-std=c++11 -O0 -ggdb)

set(SERVER_LOGIN_CPP rpc_login_server.cpp ../user.pb.cc)

add_executable(rpc_login_server ${SERVER_LOGIN_CPP})

target_link_libraries(rpc_login_server simpleRPC protobuf)