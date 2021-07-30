/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-29 15:58:27
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-29 15:58:28
 */

// 依赖rpc_header.pb.h

#include <zookeeper/zookeeper.h>
#include <string>

#include "rpc_provider.h"
#include "rpc_application.h"
#include "rpc_header.pb.h"
#include "zookeeper_util.h"

// show RPC method
void CLRPCProvider::notifyService(google::protobuf::Service* _pService){
    CServiceInfo service_info;

    const google::protobuf::ServiceDescriptor* _pServiceDesc = _pService->GetDescriptor();

    // get service name
    std::string service_name = _pServiceDesc->name();

    // get method count
    int num_of_method = _pServiceDesc->method_count();

    LOG_INFO("RPC service name: [%s]", service_name.c_str());

    for(int i = 0; i < num_of_method; ++i){
        const google::protobuf::MethodDescriptor* pMethodDesc = _pServiceDesc->method(i);
        std::string method_name = pMethodDesc->name();
        service_info.str2func[method_name] = pMethodDesc;
        LOG_INFO("RPC method name: [%s]", method_name.c_str());
    }

    service_info.pService = _pService;
    str2service[service_name] = service_info;
}


// create rpc server node
void CLRPCProvider::run(){
    std::string ip = CLRPCApplication::get_config().load(this->m_ip_key);
    int32_t port = atoi(CLRPCApplication::get_config().load(this->m_port_key).c_str());


    // create tcp server
    muduo::net::InetAddress server_addr(ip, port);
    muduo::net::TcpServer server(&m_eventLoop, server_addr, "RPC_PROVIDER_SERVER");

    // set callback
    server.setConnectionCallback(std::bind(&CLRPCProvider::OnConnect, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&CLRPCProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // num of thread
    server.setThreadNum(2);


    // create and regist rpc node
    CLZooKeeperClient zookeeper_client;
    zookeeper_client.start();

    for(auto& p : str2service){ // {service_name,(pService, {method_name, _p_method})}
        std::string service_path = "/"+p.first;
        zookeeper_client.create(service_path.c_str(), nullptr, 0);
        LOG_INFO("RPC server create node with path=[%s], value=[nullptr]", service_path.c_str());
        for(auto& pp : p.second.str2func){
            // (_pService, {method_name, _p_method})

            std::string method_path = service_path+"/"+pp.first;
            std::string zookeeper_node_value = ip+":"+std::to_string(port);
            zookeeper_client.create(method_path.c_str(), zookeeper_node_value.c_str(), zookeeper_node_value.size());
            LOG_INFO("RPC server create node with path=[%s], value=[%s]", method_path.c_str(), zookeeper_node_value.c_str());
        }
    }

    LOG_INFO("RPC server start..., ip=[%s], port=[%d]", ip.c_str(), port);
    std::cout << "RPC server start at ip=" << ip << ", port=" << port << std::endl;

    // start network service
    server.start();
    m_eventLoop.loop();

}

void CLRPCProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* _buffer, muduo::Timestamp _timestamp){
    std::string recv_msg = _buffer->retrieveAllAsString();

    LOG_INFO("RPC server recv msg, size=[%ld]", recv_msg.size());

    // rpc_header_size + rpc_header + args
    uint32_t rpc_head_size = 0;
    memcpy(&rpc_head_size, recv_msg.c_str(), sizeof(uint32_t));

    std::cout << "rpc_head_size=" << rpc_head_size << std::endl;
    std::string rpc_header_str = recv_msg.substr(sizeof(uint32_t), rpc_head_size);

    RPC::CRPCHeader rpc_header;

    if(!rpc_header.ParseFromString(rpc_header_str)){
        puts("server parse the rpc header string failed!!!");
        LOG_INFO("server parse the rpc header string [%s] failed!!!", rpc_header_str.c_str());
        return;
    }
    std::string service_name = rpc_header.service_name();
    std::string method_name = rpc_header.method_name();
    uint32_t args_len = rpc_header.args_len();

    std::string args = recv_msg.substr(sizeof(uint32_t)+rpc_head_size, args_len);

    // print some info
    std::cout << "service_name=" << service_name << std::endl;
    std::cout << "method_name=" << method_name << std::endl;
    std::cout << "args_len=" << args_len << std::endl;

    LOG_INFO("RPC client request service_name=[%s]", service_name.c_str());
    LOG_INFO("RPC client request method_name=[%s]", method_name.c_str());
    LOG_INFO("RPC client request args_len=[%ld]", args_len);


    // 根据名称找到 CServiceInfo
    auto p_service = str2service.find(service_name);
    if(p_service == str2service.end()){
        puts("RPC server do not find this service!!!");
        LOG_INFO("RPC server don't find service_name=[%s]", service_name.c_str());
        return;
    }

    auto p_method = p_service->second.str2func.find(method_name);

    google::protobuf::Service* pService = p_service->second.pService;
    const google::protobuf::MethodDescriptor* pMethodDesc = p_method->second;

    google::protobuf::Message* pRequest = pService->GetRequestPrototype(pMethodDesc).New();
    if(!pRequest->ParseFromString(args)){
        puts("RPC server parse request args failed!!!");
        LOG_INFO("RPC server parse request args=[%s] faild!!!", args.c_str());
        return;
    }

    google::protobuf::Message* pResponse = pService->GetResponsePrototype(pMethodDesc).New();

    google::protobuf::Closure* callback = google::protobuf::NewCallback<CLRPCProvider,
                                                                const muduo::net::TcpConnectionPtr&,
                                                                google::protobuf::Message*>(
                                                                    this,
                                                                    &CLRPCProvider::sendRPCResponse,
                                                                    conn,
                                                                    pResponse
                                                                );

    pService->CallMethod(pMethodDesc, nullptr, pRequest, pResponse, callback);
}


void CLRPCProvider::sendRPCResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* _response){
    std::string msg;
    if(!_response->SerializeToString(&msg)){
        puts("response convert to string failed!!!");
    }else{
        conn->send(msg);
    }
    conn->shutdown();
}
