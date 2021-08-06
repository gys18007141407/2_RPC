/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 20:09:38
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 09:09:03
 */

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <functional>
#include <unordered_map>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "Channel.h"
#include "Epoll.h"
#include "TimerQueue.h"
#include "Config.h"
#include "Helper.h"


class EventLoop{

public:
    using TASK_CALLBACK = std::function<void(void)>;
    using SptrEventLoop = std::shared_ptr<EventLoop>;
    using WptrEventLoop = std::weak_ptr<EventLoop>;

    explicit EventLoop();
    ~EventLoop();

    void loop();

    void runInLoop(TASK_CALLBACK&& runTask);

    void queueInLoop(TASK_CALLBACK&& pendingTask);

    void updateWatchEvent(std::size_t fd){
        _epoll->update(_fd2channel[fd]);
    }

    void updateWatchEvent(Channel* channel){
        _epoll->update(channel);
    }

    void addChannel(Channel* channel){
        _epoll->add(channel);
    }

    void removeChannel(std::size_t fd){
        _epoll->remove(_fd2channel[fd]);
        _fd2channel.erase(fd);
    }

    void removeChannel(Channel* channel){
        _epoll->remove(channel);
        _fd2channel.erase(channel->getFD());
    }

    void addMapping(Channel* channel){ _fd2channel[channel->getFD()] = channel; }

    std::size_t getTid() const{ return _tid; }


    void runAt(TimeStamp when, TASK_CALLBACK&& callback) {
        _timerQueue->insertTimerEvent(when, 0, std::move(callback));
    }

    void runAfter(std::size_t delay, TASK_CALLBACK&& callback) {
        _timerQueue->insertTimerEvent(TimeStamp::nowAfter(delay), 0, std::move(callback));
    }

    void runEvery(std::size_t interval, TASK_CALLBACK&& callback, std::size_t count = -1) {
        _timerQueue->insertTimerEvent(TimeStamp::now(), interval, std::move(callback), count);
    }


private:

    std::unordered_map<std::size_t, Channel*> _fd2channel;

    Epoll::UPtrEpoll _epoll;

    std::list<TASK_CALLBACK> _pendingQueue;

    std::mutex _mt_pendingQueue;

    std::condition_variable _cond_pendingQueue;

    const std::size_t _tid;

    const std::size_t _eventfd;

    Channel* _eventfdChannel;

    void asyncWakeUp();

    std::atomic<bool> _isDoingPending;

    TimerQueue* _timerQueue;
};


#endif