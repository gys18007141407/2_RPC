/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 20:53:29
 */

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include "nonCopyable.h"
#include "FixedBuffer.h"
#include <thread>
#include <string>

class LogStream:public NonCopyable{
public:

    typedef LogStream self;
    typedef FixedBuffer<kSmallSize> Buffer;

    self& operator<<(bool v){
        _buffer.append(v?"1":"0", 1);
        return *this;
    }

    self& operator<<(char v){
        _buffer.append(&v, 1);
        return *this;
    }

    self& operator<<(const char* v){
        _buffer.append(v, strlen(v));
        return *this;
    }

    self& operator<<(const unsigned char* v){
        return operator<<( reinterpret_cast<const char*>(v) );
    }
    
    self& operator<<(const std::string& v){
        _buffer.append(v.c_str(), v.size());
        return *this;
    }

    self& operator<<(const Buffer& v){
        operator<<(v.toString());
        return *this;
    }

    self& operator<<(const StringPiece& v){
        _buffer.append(v.data(), v.size());
        return *this;
    }

    self& operator<<(float v){
        return operator<<(static_cast<double>(v));
    }

    self& operator<<(double v);

    self& operator<<(short v);
    self& operator<<(unsigned short v);
    self& operator<<(int v);
    self& operator<<(unsigned int v);
    self& operator<<(long v);
    self& operator<<(unsigned long v);
    self& operator<<(long long v);
    self& operator<<(unsigned long long v);

    self& operator<<(const void* v);

    self& operator<<(const std::thread::id& id);

    bool append(const char* from, std::size_t len){
        return _buffer.append(from, len);
    }

    const Buffer& buffer() const{
        return _buffer;
    }

    void reset(){
        _buffer.reset();
    }

    template<typename T>
    void formatInteger(T v);

private:
    Buffer _buffer;

    static const int kMaxDecLength = 32;


};

#endif



