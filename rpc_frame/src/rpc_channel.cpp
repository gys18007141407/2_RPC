/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-29 15:55:42
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-29 15:55:42
 */

// 依赖 rpc_header.pb.h

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "rpc_channel.h"
#include "rpc_application.h"
#include "rpc_controller.h"
#include "zookeeper_util.h"
#include "rpc_header.pb.h"

// head_size + service_name + method_name + args_len + args

void CLRPCChannel::CallMethod(
    const google::protobuf::MethodDescriptor* _method,
    google::protobuf::RpcController* _controller,
    const google::protobuf::Message* _request,
    google::protobuf::Message* _response,
    google::protobuf::Closure* _done)
{
    const google::protobuf::ServiceDescriptor* pService = _method->service();

    std::string service_name = pService->name();
    std::string method_name = _method->name();

    uint32_t args_len = 0;
    std::string args;

    if(!_request->SerializeToString(&args)){
        puts("_request serialize failed!!!");
        return;
    }
    args_len = args.size();

    // rpc request header
    RPC::CRPCHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_len(args_len);

    uint32_t rpc_header_size = 0;
    std::string rpc_header_str;

    if(!rpc_header.SerializeToString(&rpc_header_str)){
        puts("rpc header serialize failed!!!");
        return;
    }

    rpc_header_size = rpc_header_str.size();
    // rpc request msg = head_size + head + args
    std::string rpc_request_msg = std::string(reinterpret_cast<char*>(&rpc_header_size), sizeof(uint32_t));
    rpc_request_msg += rpc_header_str;
    rpc_request_msg += args;

    std::cout << "rpc_request_msg size = " << rpc_request_msg.size() << "\t:" << rpc_request_msg << std::endl;


    // tcp connect
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == fd){
        puts("create socket failed!!!");
        _controller->SetFailed(std::string(strerror(errno)));
        return;
    }

    std::string ip;
    int32_t port = 0;

#if 1
    // do not use zookeeper

    if(service_name == "Service_User_Login_Regist"){
        ip = CLRPCApplication::get_instance().get_config().load("RPC_LOGIN_SERVER_IP");
        port = atoi(CLRPCApplication::get_instance().get_config().load("RPC_LOGIN_SERVER_PORT").c_str());
    }else{
        ip = CLRPCApplication::get_instance().get_config().load("RPC_FRIEND_SERVER_IP");
        port = atoi(CLRPCApplication::get_instance().get_config().load("RPC_FRIEND_SERVER_PORT").c_str());
    }

#endif

#if 0
    // use zookeeper
    CLZooKeeperClient zookeeper_client;
    zookeeper_client.start();

    std::string method_path = "/"+service_name+"/"+method_name;
    std::string zookeeper_node_value = zookeeper_client.get_data(method_path.c_str());
std::cout << "service_name=[" << service_name << "], method_name=[" << method_name << "]" << std::endl;
    if(zookeeper_node_value.empty()){
        _controller->SetFailed(method_name+" is NOT EXIST!!!");
        return;
    }

    int pos = zookeeper_node_value.find(':');
    if(pos == -1){
        _controller->SetFailed(method_path+" is invalid syntax");
        return;
    }

    ip = zookeeper_node_value.substr(0, pos);
    port = atoi(zookeeper_node_value.substr(pos+1).c_str());
#endif

    std::cout << "client prepare to connect server, with ip=[" << ip << "], and port=[" << port << "]" << std::endl;

    // init server sockaddr
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(-1 == (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr))){
        puts("ip assign failed");
        _controller->SetFailed(std::string(strerror(errno)));
        close(fd);
        return;
    }

    // connect server
    if(-1 == connect(fd, (sockaddr*)&server_addr, sizeof(sockaddr))){
        puts("tcp connect failed!!!");
        _controller->SetFailed(std::string(strerror(errno)));
        close(fd);
        return;
    }

    // send rpc request msg
    if(-1 == send(fd, rpc_request_msg.c_str(), rpc_request_msg.size(), 0)){
        puts("tcp send msg failed!!!");
        _controller->SetFailed(std::string(strerror(errno)));
        close(fd);
        return;
    }

    std::cout << "RPC client send msg to server, [" << rpc_request_msg << "]" << std::endl;



    // wait for server's response
    char buffer[1024];
    int len = 0;

    if(0 >= (len = recv(fd, buffer, 1024, 0))){
        puts("tcp recv failed!!!");
        _controller->SetFailed(std::string(strerror(errno)));
        close(fd);
        return;
    }

    if(!_response->ParseFromArray(buffer, len)){
        puts("wrong format msg recved from server!!!");
        _controller->SetFailed(std::string(strerror(errno)));
        close(fd);
        return;
    }else{
        std::cout << "RPC client recv msg from server, [" << buffer << "]" << std::endl; 
    }

}