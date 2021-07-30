/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-28 22:22:33
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-28 22:22:33
 */


#ifndef ZOOKEEPER_UTIL_H
#define ZOOKEEPER_UTIL_H


#include <semaphore.h>
#include <string>
#include <zookeeper/zookeeper.h>

class CLZooKeeperClient final{
private:

    zhandle_t* mp_zhandle;

public:
    explicit CLZooKeeperClient():mp_zhandle(nullptr){

    }
    ~CLZooKeeperClient(){
        if(mp_zhandle) zookeeper_close(mp_zhandle);
    }

    // start to connect to server
    void start();

    // create zknode
    void create(const char* _path, const char* _data, std::size_t _size, int32_t _state = 0);

    std::string get_data(const char*);

};


#endif