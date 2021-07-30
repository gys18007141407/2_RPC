/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-28 21:30:03
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-28 21:30:03
 */

#ifndef LOGGER_H
#define LOGGER_H



#include <string>
#include <thread>
#include "lock_queue.h"

enum ELogLevel{ DEBUG, INFO, WARN, ERROR, FATAL };

class CLLogger final{
private:
    explicit CLLogger();
    CLLogger& operator=(const CLLogger&) = delete;
    CLLogger(const CLLogger&) = delete;

    static std::size_t ms_log_level;
    static CLLockQueue<std::string> ms_q;

    static std::thread ms_writeThread;

public:
    static CLLogger& get_instance(){
        static CLLogger logger;
        return logger;
    }

    void set_log_level(const ELogLevel& _level){
        CLLogger::ms_log_level = _level;
    }

    void log(std::string _msg){
        CLLogger::ms_q.push(_msg);        // push成功就认为已经完成(假脱机)
    }

    std::size_t get_log_level() const {
        return CLLogger::ms_log_level;
    }

};


#define LOG_INFO(format, ...)                           \
    do                                                  \
    {                                                   \
        CLLogger& logger = CLLogger::get_instance();    \
        if(logger.get_log_level() > ELogLevel::INFO){   \
            break;                                      \
        }                                               \
        char msg[1024] = "[ INFO ]: ";                  \
        snprintf(msg+10, 1014, format, ##__VA_ARGS__);  \
        logger.log(msg);                                \
    }while(0);

#define LOG_ERROR(format, ...)                          \
    do                                                  \
    {                                                   \
        CLLogger& logger = CLLogger::get_instance();    \
        char msg[1024] = "[ ERROR ]: ";                 \
        snprintf(msg+11, 1013, format, ##__VA_ARGS__);  \
        logger.log(msg);                                \
    }while(0);

#endif
