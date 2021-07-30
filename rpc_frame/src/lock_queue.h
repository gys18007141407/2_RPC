/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-27 21:08:14
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-27 21:08:15
 */

#ifndef LOCK_QUEUE_H
#define LOCK_QUEUE_H

#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>

template<typename _Tp>
class CLLockQueue final{
private:
    std::mutex m_mutex;
    std::queue<_Tp> m_q;
    std::condition_variable m_cond;
    std::atomic<bool> m_quit;

public:

    CLLockQueue():m_quit(false){
        if(m_quit.load(std::memory_order_acquire)) puts("??");
    }
    ~CLLockQueue(){
        // puts("LockQueue deconstruct!!!");
        m_quit.store(true, std::memory_order_release);
        m_cond.notify_all();

        FILE* dump = fopen(".reserve.txt", "w");
        while(m_q.size()){
            std::string _ = m_q.front();
            _ = "dump: " + _ + "\n";
            m_q.pop();
            fputs(_.c_str(), dump);
        }
        fclose(dump);
    }

    void push(const _Tp& _data){
        std::lock_guard<std::mutex> g_lock(m_mutex);
        m_q.emplace(_data);
        m_cond.notify_all();
    }

    _Tp pop(){
        std::unique_lock<std::mutex> u_lock(m_mutex);
        while(m_q.empty()){
            m_cond.wait(u_lock, [this]()->bool{
                return this->m_q.size() || m_quit.load(std::memory_order_acquire);
            });
        }
        if(m_quit.load(std::memory_order_acquire)){
            return "";
        }
        _Tp data = m_q.front();
        m_q.pop();
        return data;
    }

};

#endif
