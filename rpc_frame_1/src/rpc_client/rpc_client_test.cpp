/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 14:46:52
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 14:46:52
 */

#include "rpc_client.h"
#include <iostream>

using namespace std;
void callback(int x){
    cout << "callback " << x+10 << endl;
}

int main(){

    CLRPCClient rpc_client;

    std::future<int> a = rpc_client.rpc_client_future_stub<int>("add", 1, 3);

    rpc_client.rpc_client_callback_stub<int>("add", &callback, 1, 3);

    int c = rpc_client.rpc_client_sync_stub<int>("add00", 3, 5);
    long long d = rpc_client.rpc_client_sync_stub<long long>("add", 3, 5);

    std::string concat = rpc_client.rpc_client_sync_stub<std::string>("concat", "aaabbb", "cccddd");

    cout << "a=" << a.get() << endl;
    cout << "c=" << c << endl;
    cout << "d=" << d << endl;
    cout << "concat=" << concat << endl;
    sleep(1);
    return 0;
}