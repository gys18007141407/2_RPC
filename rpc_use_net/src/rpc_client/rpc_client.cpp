/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-05 13:36:44
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-05 13:36:44
 */

#include "rpc_client.h"

CLRPCClient::CLRPCClient(const std::string& _ip, std::size_t _port, CONNECT_MODE _mode): 
    m_rpc_server_ip(_ip), 
    m_rpc_server_port(_port),
    m_mode(_mode),
    _connect_callback(std::bind(&CLRPCClient::defaultOnConnectCallBack, this, std::placeholders::_1)),
    _message_callback(std::bind(&CLRPCClient::defaultOnMessageCallBack, this, std::placeholders::_1)),
    _complete_callback(std::bind(&CLRPCClient::defaultOnCompleteCallBack, this, std::placeholders::_1)),
    _close_callback(std::bind(&CLRPCClient::defaultOnCloseCallBack, this, std::placeholders::_1))
{  
    m_rpc_server_addr.setIP(m_rpc_server_ip);
    m_rpc_server_addr.setPort(m_rpc_server_port);
}

CLRPCClient::~CLRPCClient()
{

}

bool CLRPCClient::connect(int32_t _fd){
    setFdReuse(_fd);
    sockaddr_in server_addr = this->m_rpc_server_addr.getSockAddr();
    if(-1 == ::connect(_fd, (sockaddr*)&server_addr, sizeof(sockaddr))) return false;
    return true;
}


void CLRPCClient::defaultOnConnectCallBack(const SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " RPC server OnConnect";
}
void CLRPCClient::defaultOnMessageCallBack(const SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " RPC server OnMessage";
}
void CLRPCClient::defaultOnCompleteCallBack(const SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " RPC server OnComplete";
}
void CLRPCClient::defaultOnCloseCallBack(const SptrTcpConn& _ref_sptr_conn){
    LOG_ERROR << TimeStamp::now().whenCreate_str() << " RPC server shutdown";
}