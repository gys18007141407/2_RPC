/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 20:09:17
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:51:01
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include <cstddef>
#include <unistd.h>
#include <functional>


class EventLoop;

class Channel{
public:

    using CHANNEL_READ_CALLBACK = std::function<void(void)>;
    using CHANNEL_WRITE_CALLBACK = std::function<void(void)>;

    explicit Channel(EventLoop* _loop, std::size_t _fd);                    // 生命周期由EventLoop管理
    ~Channel(){
        if(m_fd > 0) close(m_fd);
    }

    void setReadCALLBACK(CHANNEL_READ_CALLBACK&& callback){ _EPOLLIN_CALLBACK = callback; }
    void setWriteCALLBACK(CHANNEL_WRITE_CALLBACK&& callback){ _EPOLLOUT_CALLBACK = callback; }

    void runReadCallback() { _EPOLLIN_CALLBACK(); }

    void runWriteCallBack() { _EPOLLOUT_CALLBACK(); }

    void setWatchEvent(std::size_t event);

    void addWatchEvent(std::size_t event);

    void removeWatchEvent(std::size_t event);

    void setReturnEvent(std::size_t revent);

    void addToLoop();

    void removeInloop();

    std::size_t getFD(){ return m_fd; }

    std::size_t getWatchEvent(){ return m_event; }

    std::size_t getReturnEvent(){ return m_revent; }

private:
    CHANNEL_READ_CALLBACK _EPOLLIN_CALLBACK;
    CHANNEL_WRITE_CALLBACK _EPOLLOUT_CALLBACK;

    EventLoop* m_EventLoop;

    std::size_t m_fd;

    std::size_t m_event;

    std::size_t m_revent;

    bool m_inLoop;

    void defaultReadFUNC();

    void defaultWriteFUNC();

};

#endif