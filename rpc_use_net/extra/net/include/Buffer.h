/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-03 09:48:05
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 09:48:05
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <memory>
#include <cstring>
#include <string>
#include <mutex>


class Buffer final: public std::vector<char>{
private:
    std::size_t m_cur;
    bool m_thread_safe;
    std::mutex m_sync;

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    const Buffer& operator=(const Buffer&) = delete;
    const Buffer& operator=(Buffer&&) = delete;

public:

    using SptrBuf = std::shared_ptr<Buffer>;
    using Base = std::vector<char>;

    explicit Buffer(bool _thread_safe = false):m_cur(0), m_thread_safe(_thread_safe){ }

    explicit Buffer(const char* str, std::size_t size, bool _thread_safe = false): m_cur(0), m_thread_safe(_thread_safe)
    {
        Base::insert(Base::begin(), str, str+size);
    }

    ~Buffer(){ }

public:

    const char* begin(){
        if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
        return Base::data();
    }

    const char* data(){
        if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
        return Base::data()+m_cur;
    }

    std::size_t size(){
        if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
        return Base::size()-m_cur;
    }

    void offset(int k){
        if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
        if(k == this->size()) this->reset();
        else m_cur += k;
    }

    void append(const char* str, std::size_t size){
        if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
        Base::insert(Base::end(), str, str+size);
    }

    void reset(){
        if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
        m_cur = 0;
        Base::clear();
    }

    std::string retrive_get(std::size_t _len = 0);

    std::string retrive_get_all();

    std::string to_string(std::size_t _len);
}; 

#endif