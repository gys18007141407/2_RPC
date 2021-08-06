/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 20:10:37
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 22:08:08
 */

#ifndef TCPCONN_H
#define TCPCONN_H


#include <atomic>
#include "Buffer.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Config.h"


class TcpConn final : public std::enable_shared_from_this<TcpConn>{

public:

    using TASK_CALLBACK = typename EventLoop::TASK_CALLBACK;
    using SptrTcpConn = std::shared_ptr<TcpConn>;

    using ON_CONNECTION_CALLBACK = std::function<void(const SptrTcpConn&)>;
    using ON_MESSAGE_CALLBACK = std::function<void(const SptrTcpConn&)>;
    using ON_COMPLETE_CALLBACK = std::function<void(const SptrTcpConn&)>;
    using ON_CLOSE_CALLBACK = std::function<void(const SptrTcpConn&)>;

    explicit TcpConn(EventLoop* loop, std::size_t fd, bool thread_safe = false);
    ~TcpConn();

    std::string getBufferIn_all();

    std::string getBufferOut_all();

    std::string getBufferIn(std::size_t len);

    std::string getBufferOut(std::size_t len);

    std::string lookBufferIn(std::size_t len);

    std::string lookBufferOut(std::size_t len);

    std::size_t getFD() const { return _fd; }

    bool is_thread_safe() const { return _thread_safe; }

    std::size_t BufferInSize(){ return _in->size(); }
    std::size_t BufferOutSize() { return _out->size(); }

    void send(const std::string& msg);

    void runAt(TimeStamp when, TASK_CALLBACK&& callback) {
        _loop->runAt(when, std::move(callback));
    }

    // after how much micro seconds then execute
    void runAfter(std::size_t delay, TASK_CALLBACK&& callback) {
        _loop->runAfter(delay, std::move(callback));
    }

    // every micro seconds execute one time
    void runEvery(std::size_t interval, TASK_CALLBACK&& callback, std::size_t count = -1) {
        _loop->runEvery(interval, std::move(callback), count);
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

    void establish();

private:
    ON_CONNECTION_CALLBACK _connect_callback;
    ON_MESSAGE_CALLBACK _message_callback;
    ON_COMPLETE_CALLBACK _complete_callback;
    ON_CLOSE_CALLBACK _close_callback;

    SptrTcpConn _sptr_TcpConn;
    SptrTcpConn getSptrTcpConn(){  return shared_from_this(); }


private:

    void readCallBack();

    void writeCallBack();

    void sendInLoop(const std::string& msg);

    Buffer::SptrBuf _in;
    Buffer::SptrBuf _out;
    bool _thread_safe;

    EventLoop* _loop;

    std::size_t _fd;

    Channel* _channel;

    std::atomic<bool> _isChannelClosed;

};

#endif