/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-28 14:02:08
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-28 14:02:09
 */


#include <zookeeper/zookeeper.h>
#include <iostream>
int main(){


    zhandle_t* p = zookeeper_init("127.0.0.1:2181", nullptr,15000,nullptr,nullptr,0);

    std::cout << (void*)p << std::endl;
    if(!p) std::cout << "\n\nfail\n\n" << std::endl;
    else{
        std::cout << "\n\nsuccess\n\n" << std::endl;
        zookeeper_close(p);
    }

    return 0;

}