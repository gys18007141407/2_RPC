/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 14:38:41
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 14:38:41
 */

#ifndef CLNOCOPYABLE_H
#define CLNOCOPYABLE_H

class CLNoCopyable{
public:
    CLNoCopyable(){ }
    ~CLNoCopyable(){ }
private:
    CLNoCopyable(const CLNoCopyable&) = delete;
    CLNoCopyable(CLNoCopyable&&) = delete;
    CLNoCopyable& operator=(const CLNoCopyable&) = delete;
    CLNoCopyable& operator=(CLNoCopyable&&) = delete;
};

#endif