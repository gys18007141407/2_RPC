/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-29 19:04:44
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 14:22:26
 */


#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <atomic>
#include "EventLoop.h"

class EventLoopThreadPool{

public:
    using UptrEventLoopThreadPool = std::unique_ptr<EventLoopThreadPool>;
    using SptrEventLoop = EventLoop::SptrEventLoop;

    explicit EventLoopThreadPool(EventLoop* baseLoop, std::size_t num = 2);
    ~EventLoopThreadPool();

    void start();

    void stop();

    EventLoop* getNextLoop();

    std::size_t getThreadNum(){ return _num; }

private:

    std::size_t _current;

    std::vector<std::thread> _threads;

    std::size_t _num;

    EventLoop* _baseLoop;

    std::vector<SptrEventLoop> _loops;

    std::mutex _mt_loops;

    std::atomic<bool> _sync;
};


#endif