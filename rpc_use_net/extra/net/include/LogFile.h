/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 20:50:56
 */
#ifndef LOGFILE_H
#define LOGFILE_H

#include <iostream>
#include <cstddef>
#include <ctime>
#include <mutex>
#include <memory>
#include "FileUtil.h"

class LogFile:public NonCopyable{
public:
    LogFile(
        const std::string& basename,
        std::size_t rollSize,
        bool threadSafe = true,
        std::size_t flushInterval = 3,
        std::size_t checkEveryN = 1024
        
    );

    ~LogFile();

    bool append(const char* log, std::size_t len);

    void flush();

    bool rollFile();

private:

    const std::string _basename;
    const std::size_t _rollize;
    const bool _threadSafe;
    const std::size_t _flushInterval;
    const std::size_t _checkEveryN;


    bool append_unlocked(const char* log, std::size_t len);

    static std::string getLogFileName(const std::string& basename, std::time_t* now);

    std::time_t _startOfPeriod;
    std::time_t _lastRoll;
    std::time_t _lastFlush;

    std::size_t _count;

    std::mutex _mt;
    std::unique_ptr<AppendFile> _upFile;

    static const std::size_t kRollPerSeconds = 60*60*24;
};



#endif