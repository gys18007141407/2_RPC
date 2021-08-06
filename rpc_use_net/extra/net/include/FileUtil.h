/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 20:50:24
 */
#ifndef FILEUTIL_H
#define FILEUTIL_H


#include <string>
#include "nonCopyable.h"

class AppendFile: public NonCopyable{
public:
    explicit AppendFile(const char* filename);
    explicit AppendFile(std::string filename);

    ~AppendFile();

    void flush();

    bool append(const char* log, std::size_t len);

    std::size_t writtenBytes() const{
        return _writtenBytes;
    }

private:
    FILE* _fp;

    std::size_t _writtenBytes;

    char buffer[1024*64];

    std::size_t write(const char* log, std::size_t len);
};



#endif