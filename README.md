# 2_RPC

use networkLib repository to implement this rpc

<!--
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-06 08:26:19
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-06 08:26:19
-->

## 一个简单的rpc框架
基于自己的net库和c++14标准

### 待改进
实现与语言无关的序列化与反序列化(类似protobuf)<br>
这样的话，rpc_client就可以使用任何语言来调用rpc_server上注册的方法。目前只能序列化c++的内置类型和顺序容器(扩展为任何具有resize方法的容器),详细说明见rpc_base下面的serialize.h。<br/>

net库的改进<br/>
在EventLoop, TcpClient, TcpServer中设置stop函数以主动停止。<br/>

其他<br/>

---
### 使用方法

#### rpc_server

一、server端先注册函数：1、普通函数。2、成员函数。3、std::function对象<br/>
为什么不能注册lambda：因为就算对于同一个lambda函数，编译器也会认为他们是不同的函数<br>
```cpp

auto lambda_func = [](std::vector<std::vector<int>> graph, int tl, int tr)->int{
    if(graph.size() == 0 || graph[0].size() == 0) return 0;
    int m = graph.size(), n = graph[0].size();
    int res = 0;
    if(tl >= 0 && tl < m && tr >= 0 && tr < n){
        for(int i = tl; i < m; ++i){
            for(int j = tr; j < n; ++j){
                res += graph[i][j];
            }
        }
    }
    return res;
};

std::vector<std::vector<int>> m{{1,2,3}, {4,5,6}, {7,8,9}};

template<typename _Tp>
void ptype(const _Tp& _obj){
    std::cout << typeid(_obj).name() << std::endl;
}

int main()
{	
    A a;
    std::function<decltype(lambda_func({},0,0))(std::vector<std::vector<int>>, int, int)> lambda_wrap(lambda_func);

    // server.regist("matrix_sum_lambda", lambda_func);    //注册lambda
    // 为什么不能注册lambda函数：因为就算对于完全相同lambda函数，它们的类型也是不一样的。
    // 更一般的现象是，我们只能用auto来声明lambda，却不能显式指定lambda的类型，因为相同lambda可能是不同类型导致了这个现象。

    auto _ = lambda_func;

    ptype( [](){} );
    ptype( [](){} );

    ptype(lambda_func);
    ptype(_);
    ptype(lambda_wrap);

    ptype(std::function<void(void)>([](){}));
    ptype(std::function<void(void)>([](){}));


    // Z4mainEUlvE_
    // Z4mainEUlvE0_

    // UlSt6vectorIS_IiSaIiEESaIS1_EEiiE_
    // UlSt6vectorIS_IiSaIiEESaIS1_EEiiE_
    // St8functionIFiSt6vectorIS0_IiSaIiEESaIS2_EEiiEE

    // St8functionIFvvEE
    // St8functionIFvvEE
    
	return 0;
}
```

二、server运行示例

```cpp
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
```

#### rpc_client

一、rpc_server有两种调用方式：1、异步调用。2、同步调用<br/>
其中异步调用又分为两种具体方式: 1、返回future对象。2、设置回调函数(以返回值为参数)

```cpp
// 两种异步调用方式
std::future<int> a = rpc_client.rpc_client_future_stub<int>("add", 1, 3);
rpc_client.rpc_client_callback_stub<int>("add", &callback, 1, 3);

// 同步调用
int c = rpc_client.rpc_client_sync_stub<int>("add", 3, 5);
```

二、可以稍加修改以按照协议（CLResponse）返回结果，这样client就不只是得到值，还能知道调用成功与否，以及一些描述信息<br/>
这里只作为一个简单的框架供学习，下面是client代码示例。

```cpp
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
```

#### 一些日志信息
```cpp
[TRACE] => function name = "Epoll" Aug  6 08:25:39 2021 create epollfd 3 by thread 139644438858048[Epoll.cpp:24]
[INFO] => New eventLoop constructed by thread_id=[139644438858048][EventLoop.cpp:51]
[INFO] => Aug  6 08:25:39 2021 thread=[139644438858048] create a io EventLoop[EventLoopThreadPool.cpp:70]
[INFO] => Aug  6 08:25:39 2021 thread=[139644438858048] create a io EventLoop[EventLoopThreadPool.cpp:70]
[INFO] => Aug  6 08:25:39 2021 server listening..., current IP=[127.0.0.1], current port=[8888], current htonsPort=[47138][Acceptor.cpp:57]
[TRACE] => function name = "loop" Aug  6 08:25:39 2021 eventLoop start loop with thread_id=[139644438858048][EventLoop.cpp:64]
[TRACE] => function name = "Epoll" Aug  6 08:25:39 2021 create epollfd 7 by thread 139644422457088[Epoll.cpp:24]
[INFO] => New eventLoop constructed by thread_id=[139644422457088][EventLoop.cpp:51]
[TRACE] => function name = "loop" Aug  6 08:25:39 2021 eventLoop start loop with thread_id=[139644422457088][EventLoop.cpp:64]
[TRACE] => function name = "Epoll" Aug  6 08:25:39 2021 create epollfd 10 by thread 139644414064384[Epoll.cpp:24]
[INFO] => New eventLoop constructed by thread_id=[139644414064384][EventLoop.cpp:51]
[TRACE] => function name = "loop" Aug  6 08:25:39 2021 eventLoop start loop with thread_id=[139644414064384][EventLoop.cpp:64]
[TRACE] => function name = "wait" Aug  6 08:25:41 2021 fd = 6 triggered 1 event by epollfd 3[Epoll.cpp:40]
[INFO] => Aug  6 08:25:41 2021 accept a new connection with fd=[14][Acceptor.cpp:72]
[INFO] => Aug  6 08:25:41 2021 rpc client OnConnect invoked, fd=[14], do nothing[rpc_server.cpp:61]
[TRACE] => function name = "wait" Aug  6 08:25:41 2021 fd = 6 triggered 1 event by epollfd 3[Epoll.cpp:40]
[INFO] => Aug  6 08:25:41 2021 accept a new connection with fd=[15][Acceptor.cpp:72]
[INFO] => Aug  6 08:25:41 2021 rpc client OnConnect invoked, fd=[15], do nothing[rpc_server.cpp:61]
[TRACE] => function name = "wait" Aug  6 08:25:41 2021 fd = 14 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  6 08:25:41 2021 fd=[14] EPOLLIN because of read, bytes=[18][TcpConn.cpp:72]
[INFO] => Aug  6 08:25:41 2021 rpc client OnMessage invoked, fd=[14], do RPC[rpc_server.cpp:65]
[TRACE] => function name = "wait" Aug  6 08:25:41 2021 fd = 15 triggered 1 event by epollfd 10[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  6 08:25:41 2021 fd=[15] EPOLLIN because of read, bytes=[19][TcpConn.cpp:72]
[INFO] => Aug  6 08:25:41 2021 rpc client OnMessage invoked, fd=[15], do RPC[rpc_server.cpp:65]
[TRACE] => function name = "sendInLoop" Aug  6 08:25:41 2021 fd=[15] sendInLoop, msg length=[20][TcpConn.cpp:162]
[INFO] => Aug  6 08:25:41 2021 rpc client OnComplete invoked, fd=[15], do nothing[rpc_server.cpp:75]
[TRACE] => function name = "wait" Aug  6 08:25:41 2021 fd = 6 triggered 1 event by epollfd 3[Epoll.cpp:40]
[INFO] => Aug  6 08:25:41 2021 accept a new connection with fd=[16][Acceptor.cpp:72]
[INFO] => Aug  6 08:25:41 2021 rpc client OnConnect invoked, fd=[16], do nothing[rpc_server.cpp:61]
[TRACE] => function name = "sendInLoop" Aug  6 08:25:46 2021 fd=[14] sendInLoop, msg length=[20][TcpConn.cpp:162]
[INFO] => Aug  6 08:25:46 2021 rpc client OnComplete invoked, fd=[14], do nothing[rpc_server.cpp:75]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 16 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  6 08:25:46 2021 fd=[16] EPOLLIN because of read, bytes=[19][TcpConn.cpp:72]
[INFO] => Aug  6 08:25:46 2021 rpc client OnMessage invoked, fd=[16], do RPC[rpc_server.cpp:65]
[TRACE] => function name = "sendInLoop" Aug  6 08:25:46 2021 fd=[16] sendInLoop, msg length=[20][TcpConn.cpp:162]
[INFO] => Aug  6 08:25:46 2021 rpc client OnComplete invoked, fd=[16], do nothing[rpc_server.cpp:75]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 6 triggered 1 event by epollfd 3[Epoll.cpp:40]
[INFO] => Aug  6 08:25:46 2021 accept a new connection with fd=[17][Acceptor.cpp:72]
[INFO] => Aug  6 08:25:46 2021 rpc client OnConnect invoked, fd=[17], do nothing[rpc_server.cpp:61]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 17 triggered 1 event by epollfd 10[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  6 08:25:46 2021 fd=[17] EPOLLIN because of read, bytes=[21][TcpConn.cpp:72]
[INFO] => Aug  6 08:25:46 2021 rpc client OnMessage invoked, fd=[17], do RPC[rpc_server.cpp:65]
[WARN] => Aug  6 08:25:46 2021 RPC client call a non-existent method name=[add00][rpc_server.cpp:49]
[TRACE] => function name = "sendInLoop" Aug  6 08:25:46 2021 fd=[17] sendInLoop, msg length=[70][TcpConn.cpp:162]
[INFO] => Aug  6 08:25:46 2021 rpc client OnComplete invoked, fd=[17], do nothing[rpc_server.cpp:75]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 6 triggered 1 event by epollfd 3[Epoll.cpp:40]
[INFO] => Aug  6 08:25:46 2021 accept a new connection with fd=[18][Acceptor.cpp:72]
[INFO] => Aug  6 08:25:46 2021 rpc client OnConnect invoked, fd=[18], do nothing[rpc_server.cpp:61]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 18 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  6 08:25:46 2021 fd=[18] EPOLLIN because of read, bytes=[19][TcpConn.cpp:72]
[INFO] => Aug  6 08:25:46 2021 rpc client OnMessage invoked, fd=[18], do RPC[rpc_server.cpp:65]
[TRACE] => function name = "sendInLoop" Aug  6 08:25:46 2021 fd=[18] sendInLoop, msg length=[20][TcpConn.cpp:162]
[INFO] => Aug  6 08:25:46 2021 rpc client OnComplete invoked, fd=[18], do nothing[rpc_server.cpp:75]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 6 triggered 1 event by epollfd 3[Epoll.cpp:40]
[INFO] => Aug  6 08:25:46 2021 accept a new connection with fd=[19][Acceptor.cpp:72]
[INFO] => Aug  6 08:25:46 2021 rpc client OnConnect invoked, fd=[19], do nothing[rpc_server.cpp:61]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 19 triggered 1 event by epollfd 10[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  6 08:25:46 2021 fd=[19] EPOLLIN because of read, bytes=[42][TcpConn.cpp:72]
[INFO] => Aug  6 08:25:46 2021 rpc client OnMessage invoked, fd=[19], do RPC[rpc_server.cpp:65]
[TRACE] => function name = "sendInLoop" Aug  6 08:25:46 2021 fd=[19] sendInLoop, msg length=[36][TcpConn.cpp:162]
[INFO] => Aug  6 08:25:46 2021 rpc client OnComplete invoked, fd=[19], do nothing[rpc_server.cpp:75]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 18 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 16 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 14 triggered 1 event by epollfd 7[Epoll.cpp:40]
[INFO] => Aug  6 08:25:46 2021 fd=[18] EPOLLIN but disconnect, remove the fd![TcpConn.cpp:64]
[INFO] => Aug  6 08:25:46 2021 rpc client OnClose invoked, fd=[18], erase from TcpServer[rpc_server.cpp:79]
[INFO] => Aug  6 08:25:46 2021 fd=[16] EPOLLIN but disconnect, remove the fd![TcpConn.cpp:64]
[INFO] => Aug  6 08:25:46 2021 rpc client OnClose invoked, fd=[16], erase from TcpServer[rpc_server.cpp:79]
[INFO] => Aug  6 08:25:46 2021 fd=[14] EPOLLIN but disconnect, remove the fd![TcpConn.cpp:64]
[INFO] => Aug  6 08:25:46 2021 rpc client OnClose invoked, fd=[14], erase from TcpServer[rpc_server.cpp:79]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 19 triggered 1 event by epollfd 10[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 17 triggered 1 event by epollfd 10[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  6 08:25:46 2021 fd = 15 triggered 1 event by epollfd 10[Epoll.cpp:40]
[INFO] => Aug  6 08:25:46 2021 fd=[19] EPOLLIN but disconnect, remove the fd![TcpConn.cpp:64]
[INFO] => Aug  6 08:25:46 2021 rpc client OnClose invoked, fd=[19], erase from TcpServer[rpc_server.cpp:79]
[INFO] => Aug  6 08:25:46 2021 fd=[17] EPOLLIN but disconnect, remove the fd![TcpConn.cpp:64]
[INFO] => Aug  6 08:25:46 2021 rpc client OnClose invoked, fd=[17], erase from TcpServer[rpc_server.cpp:79]
[INFO] => Aug  6 08:25:46 2021 fd=[15] EPOLLIN but disconnect, remove the fd![TcpConn.cpp:64]
[INFO] => Aug  6 08:25:46 2021 rpc client OnClose invoked, fd=[15], erase from TcpServer[rpc_server.cpp:79]

```
