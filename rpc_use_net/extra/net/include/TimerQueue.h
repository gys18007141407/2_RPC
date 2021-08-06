/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 19:56:13
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:50:30
 */

#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <set>
#include <vector>
#include "Timer.h"

class EventLoop;

class TimerQueue{

public:
    typedef Timer::TIMER_CALLBACK TIMER_CALLBACK;
    typedef std::pair<TimeStamp, Timer*> TT;

    using UptrTimerQueue = std::unique_ptr<TimerQueue>;

    explicit TimerQueue(EventLoop* loop);    // loop用来创建channel，listen_eventloop生命周期由用户管理，io_eventloop生命周期由listen_eventloop管理

    ~TimerQueue() = default;

    std::vector<TT> getOverTimeEvent(TimeStamp now);

    void insertTimerEvent(TimeStamp timeStamp, std::size_t interval, TIMER_CALLBACK&& callback, std::size_t count = -1);

    std::size_t getTimerfd(){
        return _tfd;
    }


private:

    std::set<TT> _TimerEvents;

    EventLoop* _loop;

    std::size_t _tfd;


    timespec howFarFromNow(TimeStamp nextTrigger);

    void setTimerfd(TimeStamp nextTrigger);
  
    void removeTimerEvent(Timer* ptimer);

    bool insertTimerPointer(Timer* ptimer);

};

#endif