/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 17:32:13
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 17:32:13
 */

#include "rpc_server.h"


int add(int x, int y){
    return x+y;
}

class A{
public:
    std::string add(std::string x, std::string y){
        return x+y;
    }
};

int main(){

    CLRPCServer rpc_server;

    A a;
    rpc_server.regist(add, "add");
    rpc_server.regist(&A::add, &a, "concat");

    rpc_server.run();
    return 0;
}