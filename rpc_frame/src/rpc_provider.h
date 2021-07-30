/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-28 22:29:01
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-28 22:29:01
 */

#ifndef RPC_PROVIDER_H
#define RPC_PROVIDER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Timestamp.h>

#include <google/protobuf/descriptor.h>

#include <unordered_map>
#include <string>
#include <functional>

#include "google/protobuf/service.h"
#include "logger.h"

// rpc service
class CLRPCProvider final{
public:
    CLRPCProvider() = delete;

    CLRPCProvider(const std::string& _ip_key, const std::string _port_key){
        m_ip_key = _ip_key;
        m_port_key = _port_key;
    }

    ~CLRPCProvider(){

    }

    // publich rpc method interface
    void notifyService(google::protobuf::Service* _pService);

    // start rpc service node
    void run();

private:
    // ip and port key
    std::string m_ip_key;
    std::string m_port_key;

    // eventloop
    muduo::net::EventLoop m_eventLoop;

    // service info
    struct CServiceInfo{
        google::protobuf::Service* pService;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> str2func;
    };

    // store <service, service::method>
    std::unordered_map<std::string, CServiceInfo> str2service;

    // OnConnect callback
    void OnConnect(const muduo::net::TcpConnectionPtr& conn){
        if(!conn->connected()){
            conn->shutdown();
        }
    }

    // OnMessage callback
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

    // Closure callback
    void sendRPCResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* _response);

};

#endif