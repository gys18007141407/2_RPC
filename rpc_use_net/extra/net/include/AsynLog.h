/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 20:50:14
 */
#ifndef ASYNLOG_H
#define ASYNLOG_H


#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <condition_variable>
#include "LogStream.h"

class AsynLog:public NonCopyable{

public:

    AsynLog(
        const std::string& basename,
        std::size_t rollSize = 1024,
        std::size_t flushInterval = 3
    );

    ~AsynLog(){
        if(_running){
            stop();
        }
    }

    bool append(const char* log, std::size_t len);

    void start(){
        _running = true;

        _pthread = new std::thread(&AsynLog::threadFUNC, this);
    }

    void stop(){
        _running = false;

        _cond.notify_all();

        _pthread->join();

    }

private:
    std::atomic<bool> _running;

    void threadFUNC();

    std::thread* _pthread;

    std::condition_variable _cond;

    std::mutex _mt;


    const std::string _basename;
    const std::size_t _rollSize;
    const std::size_t _flushInterval;

    typedef FixedBuffer<kBigSize> Buffer; 
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;


    BufferPtr _curBuffer;
    BufferPtr _nextBuffer;
    BufferVector _pBuffers;


};



#endif