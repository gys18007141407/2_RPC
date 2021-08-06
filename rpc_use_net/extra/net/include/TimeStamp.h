/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 19:57:13
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:44:57
 */


#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <cstddef>
#include <string>
#include <sys/time.h>

class TimeStamp{
public:
    static const std::size_t kMiliSec = 1000;
    static const std::size_t kMicroSec = 1000*1000;

    explicit TimeStamp(std::size_t t = 0) : _createTime(t){

    }

    static TimeStamp now();

    static TimeStamp nowAfter(std::size_t microSecDelay);

    static std::size_t now_t();

    std::size_t whenCreate() const{
        return _createTime;
    }

    std::string whenCreate_str() const{
        char date[32];
        std::size_t s = _createTime / kMicroSec;
        tm* t = localtime(reinterpret_cast<time_t*>(&s));
        int n = snprintf(date, 32, "%s", asctime(t));
        return std::string(date+4, 20);
    }

    bool isValid(){ return _createTime > 0; }

private:

    std::size_t _createTime;


};

bool operator<(const TimeStamp&x, const TimeStamp&y);
bool operator>(const TimeStamp&x, const TimeStamp&y);
bool operator==(const TimeStamp&x, const TimeStamp&y);
bool operator!=(const TimeStamp&x, const TimeStamp&y);
bool operator<=(const TimeStamp&x, const TimeStamp&y);
bool operator>=(const TimeStamp&x, const TimeStamp&y);

#endif