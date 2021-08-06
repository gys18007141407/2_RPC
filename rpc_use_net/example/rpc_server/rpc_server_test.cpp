/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 17:32:13
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 17:32:13
 */

#include "rpc_server.h"
#include "AsynLog.h"

int add(int x, int y){
    return x+y;
}

int time_waste(){
    sleep(5);
    return 0;
}

class A{
public:
    std::string add(std::string x, std::string y){
        return x+y;
    }
};


AsynLog g_log(__FILE__, 1024*1024, 1);
Logging::LogLevel g_logLevel = Logging::LogLevel::TRACE;

void logRedirect(const char* _data, std::size_t _len){
    g_log.append(_data, _len);
}

int main(){

    Logging::setOutFunc(logRedirect);
    g_log.start();

    CLRPCServer rpc_server("127.0.0.1", 8888, 4);

    A a;
    rpc_server.regist(add, "add");
    rpc_server.regist(&A::add, &a, "concat");

    rpc_server.regist(time_waste, "time_waste");

    rpc_server.run();
    return 0;
}