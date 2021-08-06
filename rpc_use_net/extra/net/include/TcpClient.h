/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-03 21:24:57
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 21:24:57
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "InetAddr.h"
#include "Buffer.h"
#include "TcpConn.h"
#include "Logging.h"

class TcpClient{
public:
    using UptrTcpClient = std::unique_ptr<TcpClient>;
    using SptrTcpClient = std::shared_ptr<TcpClient>;
    using SptrTcpConn = typename TcpConn::SptrTcpConn;

    using ON_CONNECTION_CALLBACK = typename TcpConn::ON_CONNECTION_CALLBACK;
    using ON_MESSAGE_CALLBACK = typename TcpConn::ON_MESSAGE_CALLBACK;
    using ON_COMPLETE_CALLBACK = typename TcpConn::ON_COMPLETE_CALLBACK;
    using ON_CLOSE_CALLBACK = typename TcpConn::ON_CLOSE_CALLBACK;

    explicit TcpClient(EventLoop* loop, const std::string& ip = "127.0.0.1", std::size_t port = 8888, bool thread_safe = true);

    ~TcpClient();

public:

    bool connect();   // 建立reset与server通信的TcpConn（作为自己的私有成员，生命周期由TcpClient管理）

    bool is_connected() const { return this->_is_connect; }

    void setOnConnectCallBack(ON_CONNECTION_CALLBACK&& _callback){
        _connect_callback = _callback;
    }
    void setOnMessageCallBack(ON_MESSAGE_CALLBACK&& _callback){
        _message_callback = _callback;
    }
    void setOnCompleteCallBack(ON_COMPLETE_CALLBACK&& _callback){
        _complete_callback = _callback;
    }
    void setOnCloseCallBack(ON_CLOSE_CALLBACK&& _callback){
        _close_callback = _callback;
    }

private:

    void defaultOnConnectCallBack(const SptrTcpConn& _ref_sptr_conn);
    void defaultOnMessageCallBack(const SptrTcpConn& _ref_sptr_conn);
    void defaultOnCompleteCallBack(const SptrTcpConn& _ref_sptr_conn);
    void defaultOnCloseCallBack(const SptrTcpConn& _ref_sptr_conn);

    ON_CONNECTION_CALLBACK _connect_callback;
    ON_MESSAGE_CALLBACK _message_callback;
    ON_COMPLETE_CALLBACK _complete_callback;
    ON_CLOSE_CALLBACK _close_callback;

public:
    void setIP(const std::string& ip){
        _server_addr.setIP(ip);
    }

    void setPort(std::size_t port){
        _server_addr.setPort(port);
    }

    std::string getIP() const{
        return _server_addr.getIP();
    }

    int32_t getPort() const {
        return _server_addr.getPort();
    }

    sockaddr_in getSockAddr(){
        return _server_addr.getSockAddr();
    }

    void send(const std::string& msg);
    

private:
    InetAddr _server_addr;
    int32_t _fd;
    Buffer _in;
    Buffer _out;

    bool _is_connect;
    bool _thread_safe;

    TcpConn::SptrTcpConn _sptr_TcpConn; // 生命周期由TcpClient控制
    EventLoop* _loop;                   // 生命周期由用户控制
    
};


#endif