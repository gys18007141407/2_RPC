/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-29 17:02:07
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 09:01:22
 */

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <sys/types.h>
#include <cstddef>
#include <functional>
#include "InetAddr.h"
#include "Config.h"
#include "Helper.h"


class Channel;
class EventLoop;

class Acceptor{

public:
    typedef std::function<void(std::size_t)> ACCEPT_CALLBACK;

    using UptrAcceptor = std::unique_ptr<Acceptor>;

    explicit Acceptor(EventLoop* loop);  // 这个loop是由用户管理的
    ~Acceptor();

    
    void setIP(const char* IP){ _addr.setIP(IP); }

    void setIP(std::string IP) { _addr.setIP(IP); }

    void setPort(std::size_t port){ _addr.setPort(port); }

    void start();

    void setAcceptCALLBACK(ACCEPT_CALLBACK&& callback){ _accept_callback = callback; }


private:

    bool init();

    sockaddr_in _sockaddr;

    InetAddr _addr;

    std::size_t _listenfd;

    Channel* _listenChannel;

    EventLoop* _listenLoop;

    ACCEPT_CALLBACK _accept_callback;
};

#endif