/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-23 19:57:50
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-24 18:40:42
 */

#ifndef CLBUFFERVEC_H
#define CLBUFFERVEC_H

#include <vector>
#include <memory>
#include <cstring>
#include <string>

// 包裹一个vector<char>
class CLBuffer final: public std::vector<char>{
private:
    std::size_t m_cur;

    CLBuffer(const CLBuffer&) = delete;
    CLBuffer(CLBuffer&&) = delete;
    const CLBuffer& operator=(const CLBuffer&) = delete;
    const CLBuffer& operator=(CLBuffer&&) = delete;

public:

    using CLSptrBuf = std::shared_ptr<CLBuffer>;
    using CLBase = std::vector<char>;

    explicit CLBuffer():m_cur(0){ }

    explicit CLBuffer(const char* str, std::size_t size): m_cur(0)
    {
        CLBase::insert(CLBase::begin(), str, str+size);
    }

    ~CLBuffer(){ }

    const char* data(){
        return CLBase::data();
    }

    const char* curData(){
        return this->data()+m_cur;
    }

    std::size_t curSize() const {
        return CLBase::size()-m_cur;
    }

    void offset(int k){
        m_cur += k;
    }

    void append(const char* str, std::size_t size){
        CLBase::insert(CLBase::end(), str, str+size);
    }

    void reset(){
        m_cur = 0;
        CLBase::clear();
    }

}; 












#endif
