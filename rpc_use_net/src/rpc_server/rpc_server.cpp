/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-04 16:55:42
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-04 16:55:42
 */

#include "rpc_server.h"


CLRPCServer::CLRPCServer(const std::string& _ip, int32_t _port, int32_t _num_thread):
    m_rpc_server_ip(_ip),
    m_rpc_server_port(_port),
    m_num_thread(_num_thread),
    m_connect_callback(std::bind(&CLRPCServer::defaultOnConnectCallBack, this, std::placeholders::_1)),
    m_message_callback(std::bind(&CLRPCServer::defaultOnMessageCallBack, this, std::placeholders::_1)),
    m_complete_callback(std::bind(&CLRPCServer::defaultOnCompleteCallBack, this, std::placeholders::_1)),
    m_close_callback(std::bind(&CLRPCServer::defaultOnCloseCallBack, this, std::placeholders::_1))
{

}

CLRPCServer::~CLRPCServer(){

}

// rpc_server_stub
// 接收来自rpc_client序列化后的数据构造CLSerializer。反序列化出rpc_client想要调用的方法和参数,进行方法调用并返回序列化结果
CLSerializer::CLSptrSlz CLRPCServer::rpc_server_stub(const char* _data, std::size_t _size){
    // 定义返回结果
    CLSerializer::CLSptrSlz sp_Slz_response = std::make_shared<CLSerializer>();

    // 转为序列化数据
    CLSerializer slz_request(_data, _size);

    // 获取函数名称
    std::string func_name;
    slz_request >> func_name;

    // 查找注册方法
    if(this->m_name2func.count(func_name)){
        auto func = this->m_name2func[func_name];
        func(slz_request.data(), slz_request.size(), sp_Slz_response.get());
    }else{
        // LOG
        // rpc_server没有此方法
        LOG_WARN << TimeStamp::now().whenCreate_str() << " RPC client call a non-existent method name=[" << func_name << "]";
        CLRPCResult<bool> rpc_result;
        rpc_result.set_state_code(ERR_CODE::RPC_NOT_FIND);
        rpc_result.set_state_desc("RPC server do not have this method which name=[" + func_name + "]");
        *sp_Slz_response.get() << rpc_result;
    }

    // 返回固定结构的消息
    return sp_Slz_response;
}

void CLRPCServer::defaultOnConnectCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " rpc client OnConnect invoked, fd=[" << _ref_sptr_conn->getFD() << "], do nothing";
}

void CLRPCServer::defaultOnMessageCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " rpc client OnMessage invoked, fd=[" << _ref_sptr_conn->getFD() << "], do RPC";

    std::string msg = _ref_sptr_conn->getBufferIn_all();

    CLSerializer::CLSptrSlz sptrSlz = this->rpc_server_stub(msg.c_str(), msg.size());

    _ref_sptr_conn->send(sptrSlz->to_string());
}

void CLRPCServer::defaultOnCompleteCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " rpc client OnComplete invoked, fd=[" << _ref_sptr_conn->getFD() << "], do nothing";
}

void CLRPCServer::defaultOnCloseCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " rpc client OnClose invoked, fd=[" << _ref_sptr_conn->getFD() << "], erase from TcpServer";
    m_uptr_server->eraseSptrTcpConn(_ref_sptr_conn);
}

