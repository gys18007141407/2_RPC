/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 20:10:52
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 13:32:17
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <cstddef>

#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "TcpConn.h"

class EventLoop;

class TcpServer{

public:
    using UptrTcpServer = std::unique_ptr<TcpServer>;
    using SptrTcpConn = typename TcpConn::SptrTcpConn;
    using NEW_CONNECT_CALLBACK = typename Acceptor::ACCEPT_CALLBACK;  // accept的回调函数

    using ON_CONNECTION_CALLBACK = typename TcpConn::ON_CONNECTION_CALLBACK;
    using ON_MESSAGE_CALLBACK = typename TcpConn::ON_MESSAGE_CALLBACK;
    using ON_COMPLETE_CALLBACK = typename TcpConn::ON_COMPLETE_CALLBACK;
    using ON_CLOSE_CALLBACK = typename TcpConn::ON_CLOSE_CALLBACK;

    explicit TcpServer(EventLoop* pLoop, const std::string& IP = "127.0.0.1", const std::size_t port = 8888, const std::size_t threads = 2);

    ~TcpServer();

    void start();

    std::size_t getThreadNum(){ return m_Uptr_ioEventLoops->getThreadNum(); }

    void setIP(const char* IP){ m_Uptr_Acceptor->setIP(IP); }

    void setIP(std::string IP) { m_Uptr_Acceptor->setIP(IP); }

    void setPort(std::size_t port){ m_Uptr_Acceptor->setPort(port); }

    void setAcceptCallBack(NEW_CONNECT_CALLBACK&& _callback){      // void(std::size_t fd)
        this->_accept_callback = _callback;
    }

    void setConnectCallBack(ON_CONNECTION_CALLBACK&& _callback){
        this->_connect_callback = _callback;
    }

    void setMessageCallBack(ON_MESSAGE_CALLBACK&& _callback){
        this->_message_callback = _callback;
    }

    void setCompleteCallBack(ON_COMPLETE_CALLBACK&& _callback){
        this->_complete_callback = _callback;
    }

    void setCloseCallBack(ON_CLOSE_CALLBACK&& _callback){
        this->_close_callback = _callback;
    }

//  TcpConn互斥访问
    void eraseSptrTcpConn(const SptrTcpConn& _sptr_TcpConn){
        std::lock_guard<std::mutex> g_lock(this->m_mt_conns);
        this->m_conns.erase(_sptr_TcpConn);
    }

    void addSptrTcpConn(const SptrTcpConn& _sptr_TcpConn){
        std::lock_guard<std::mutex> g_lock(this->m_mt_conns);
        this->m_conns.insert(_sptr_TcpConn);
    }

    template<typename _Tp, typename std::enable_if< std::is_same<typename _Tp::value_type, SptrTcpConn>::value, int>::type N = 0>
    void getAllTcpConn(_Tp& container){
        std::lock_guard<std::mutex> g_lock(this->m_mt_conns);
        container = _Tp{this->m_conns.begin(), this->m_conns.end()};
    }

private:

    EventLoop* m_listenEventLoop;                                       // 生命周期由用户管理

    EventLoopThreadPool::UptrEventLoopThreadPool m_Uptr_ioEventLoops;   // unique_ptr 生命周期由TcpServer类管理
    Acceptor::UptrAcceptor m_Uptr_Acceptor;                             // unique_ptr 生命周期由TcpServer类管理

    void defaultAcceptCallBack(std::size_t fd);
    void defaultConnectCallBack(const TcpConn::SptrTcpConn& ref_sptr_conn);
    void defaultMessageCallBack(const TcpConn::SptrTcpConn& ref_sptr_conn);
    void defaultCompleteCallBack(const TcpConn::SptrTcpConn& ref_sptr_conn);
    void defaultCloseCallBack(const TcpConn::SptrTcpConn& ref_sptr_conn);

    std::set<TcpConn::SptrTcpConn> m_conns;                             // 管理TcpConn的生命周期
    std::mutex m_mt_conns;
private:
    NEW_CONNECT_CALLBACK _accept_callback;

    ON_CONNECTION_CALLBACK _connect_callback;
    ON_MESSAGE_CALLBACK _message_callback;
    ON_COMPLETE_CALLBACK _complete_callback;
    ON_CLOSE_CALLBACK _close_callback;

};


#endif