/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 19:57:13
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:44:51
 */

#ifndef TIMER_H
#define TIMER_H


#include <functional>
#include <future>
#include <atomic>

#include "TimeStamp.h"

class Timer{

public:

    typedef std::function<void(void)> TIMER_CALLBACK;

    explicit Timer(TimeStamp timeStamp, std::size_t interval = 0, TIMER_CALLBACK&& callback = {}, std::size_t count = -1);

    ~Timer() = default;

    bool isRepeat(){ return _interval > 0;}

    void run(){ _callback(); }

    void moveInterval();

    void unRepeat(){ _interval = 0; }

    void setCallBack(TIMER_CALLBACK&& callback){ _callback = callback; }

    TimeStamp getTimeStamp(){ return _timeStamp; }

private:

    TimeStamp _timeStamp;

    std::size_t _interval;

    TIMER_CALLBACK _callback;

    std::atomic<std::size_t> _count;


private:

    Timer(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = delete;

};


#endif