/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 14:46:52
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 14:46:52
 */

#include "rpc_client.h"
#include "Logging.h"
#include <iostream>

using namespace std;
void callback(int x){
    cout << "callback " << x+10 << endl;
}

Logging::LogLevel g_logLevel = Logging::LogLevel::WARN;

int main(){

    CLRPCClient rpc_client;

    std::future<int> t = std::move(rpc_client.rpc_client_future_stub<int>("time_waste"));
    puts("time_waste skiped, Async execute...");
    
    std::future<int> a = rpc_client.rpc_client_future_stub<int>("add", 1, 3);
    cout << "a=" << a.get() << endl;
    
    rpc_client.rpc_client_callback_stub<int>("add", &callback, 1, 3);

    int c = rpc_client.rpc_client_sync_stub<int>("add00", 3, 5);            // 函数名错误
    cout << "c=" << c << endl;

    long long d = rpc_client.rpc_client_sync_stub<long long>("add", 3, 5);  // 函数参数类型错误，返回参数类型也错误
    cout << "d=" << d << endl;


    std::string concat = rpc_client.rpc_client_sync_stub<std::string>("concat", "aaabbb", "cccddd");


    cout << "concat=" << concat << endl;

    cout << "t=" << t.get() << endl;
    return 0;
}